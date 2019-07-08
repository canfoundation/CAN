#include <appbase/application.hpp>
#include <eosio/chain_plugin/chain_plugin.hpp>
#include <eosio/chain/config.hpp>
#include <eosio/producer_plugin/producer_plugin.hpp>

#include <boost/program_options.hpp>

using namespace appbase;
using namespace eosio;
using namespace eosio::chain;
using namespace std;

namespace bpo = boost::program_options;
using bpo::options_description;
using bpo::variables_map;

enum return_codes {
   FAIL     = -1,
   SUCCESS  = 0,
};


void add_block_with_regproducer_trxs( controller& chain, const private_key_type& priv_key, const public_key_type& pub_key ) {
   auto abi = chain.get_account(N(eosio)).get_abi();
   chain::abi_serializer abis(abi, fc::microseconds::maximum());
   string action_type_name = abis.get_action_type(N(regproducer));
   FC_ASSERT( action_type_name != string(), "regproducer action is not found in the chain");

   std::cerr << "ADD BLOCK WITH REGPRODUCER" << chain.head_block_num() << std::endl;
   auto next_time = chain.head_block_time() + fc::milliseconds(config::block_interval_ms);
   chain.start_block( next_time, chain.head_block_num() - chain.last_irreversible_block_num(), {} );

   for (auto& prod: chain.head_block_state()->active_schedule.producers ) {
      ilog("Call regproducer for ${n}", ("n", prod.producer_name));
      action act;
      act.account = N(eosio);
      act.name = N(regproducer);
      act.authorization = {{prod.producer_name, config::active_name}};
      act.data = abis.variant_to_binary( action_type_name,  
                                          mutable_variant_object()
                                             ("producer",      prod.producer_name)
                                             ("producer_key",  string(pub_key))
                                             ("url",           "")
                                             ("location",      0), 
                                          fc::microseconds::maximum());

      signed_transaction trx;
      trx.actions.emplace_back( act );
      trx.expiration = next_time + fc::seconds(300);
      trx.set_reference_block( chain.head_block_id() );
      trx.max_net_usage_words = 0; 
      trx.max_cpu_usage_ms = 0;
      trx.delay_sec = 0;
      trx.sign( priv_key, chain.get_chain_id() );

      auto mtrx = std::make_shared<transaction_metadata>(trx);
      transaction_metadata::start_recover_keys( mtrx, chain.get_thread_pool(), chain.get_chain_id(), fc::microseconds::maximum() );
      auto result = chain.push_transaction( mtrx, fc::time_point::maximum() );
      if( result->except_ptr ) std::rethrow_exception( result->except_ptr );
      if( result->except)  throw *result->except;
   }

   chain.finalize_block( [&]( const digest_type& d ) {
      return priv_key.sign(d);
   } );

   chain.commit_block();
}

snapshot_reader_ptr read_snapshot( const string& snapshot_input_path ) {
   auto infile = std::ifstream(snapshot_input_path, (std::ios::in | std::ios::binary));
   auto reader = std::make_shared<istream_snapshot_reader>(infile);
   reader->validate();
   return reader;
}

void create_snapshot( controller& chain, const string& snapshot_output_path ) {
   auto snap_out = std::ofstream(snapshot_output_path, (std::ios::out | std::ios::binary));
   auto writer = std::make_shared<ostream_snapshot_writer>(snap_out);
   chain.write_snapshot(writer);
   writer->finalize();
   snap_out.flush();
   snap_out.close();
}

int main(int argc, char** argv) {
   int return_code = SUCCESS;

   // Define the temp blocks and states directory, and ensure they are empty
   fc::temp_directory tempdir;
   auto blocks_dir = tempdir.path() / std::string("eosio-snapshot-modifier-").append(config::default_blocks_dir_name);
   auto state_dir = tempdir.path() / std::string("eosio-snapshot-modifier-").append(config::default_state_dir_name);
   fc::remove_all(blocks_dir);
   fc::remove_all(state_dir);

   try {
      auto priv_key = private_key_type::regenerate<fc::ecc::private_key_shim>(fc::sha256::hash(std::string("hahaha")));
      auto pub_key = priv_key.get_public_key();
      string signature_provider = string(pub_key) + "=KEY:" + string(priv_key);
      
      std::cerr << "SIGN PROVIDER NAME" << signature_provider << std::endl;
      string snapshot_input_path = "/Users/andrianto.lie/Downloads/snapshot-00000167d35ac4f6a25af8de6bce025cb7e90da0b4074e261a6c96e2d65ad1c4.bin";
      string snapshot_output_path = "/Users/andrianto.lie/Downloads/snapshot-haha.bin";
      
      controller::config cfg;
      cfg.blocks_dir = blocks_dir;
      cfg.state_dir = state_dir;
      cfg.state_size = 1024*1024*8;
      cfg.state_guard_size = 0;
      cfg.reversible_cache_size = 1024*1024*8;
      cfg.reversible_guard_size = 0;

      // Create a new chain from the snapshot
      controller chain(cfg, {});
      chain.add_indices();
      chain.startup( []() { return false; }, read_snapshot(snapshot_input_path) );

      // Replace producer keys with the new key
      chain.replace_producer_keys( pub_key );

      // Add a block with regproducer called for each producer
      add_block_with_regproducer_trxs( chain, priv_key, pub_key );

      // Create the snapshot
      create_snapshot(chain, snapshot_output_path);

   }  catch( const fc::exception& e ) {
      elog( "${e}", ("e", e.to_detail_string()));
      return_code = FAIL;
   } catch( const std::exception& e ) {
      elog("Caught Exception: ${e}", ("e",e.what()));
      return_code = FAIL;
   } catch( ... ) {
      elog("Unknown exception");
      return_code = FAIL;
   } finally

   // Clean the temporary blocks and states dir
   fc::remove_all(blocks_dir);
   fc::remove_all(state_dir);

   ilog("${name} successfully exiting", ("name", "eosio-snapshot-modifier"));
   return return_code;
}
