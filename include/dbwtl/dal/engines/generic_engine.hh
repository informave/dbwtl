//
// generic_engine.hh - Generic Engine
//
// Copyright (C)         informave.org
//   2010,               Daniel Vogelbacher <daniel@vogelbacher.name>
//
// BSD License
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//
// Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//
// Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in the
// documentation and/or other materials provided with the
// distribution.
//
// Neither the name of the copyright holders nor the names of its
// contributors may be used to endorse or promote products derived
// from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
// PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE REGENTS OR
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//

/// @file
/// @brief Generic Engine
/// @author Daniel Vogelbacher
/// @since 0.0.1


#ifndef INFORMAVE_DB_DAL_GENERIC_ENGINE_HH
#define INFORMAVE_DB_DAL_GENERIC_ENGINE_HH

#include "dbwtl/dal/dal_fwd.hh"
#include "dbwtl/dal/dal_interface.hh"
#include "dbwtl/db_objects.hh"
#include "dbwtl/util/smartptr.hh"


DAL_NAMESPACE_BEGIN

// forward
struct generic;

/// parse a driver string
std::map<std::string, String::Internal> parse_driver(const String::Internal &str);






struct generic_sqlstates : public basic_states
{
    virtual ~generic_sqlstates(void)
    {}


    // dynamic SQL error
    typedef sqlstate<DAL_SQLSTATE_07000>                         SQLSTATE_07000;
    typedef sqlstate<DAL_SQLSTATE_07000>                         SQLSTATE_dynamic_sql_error;
    
// using clause does not match dynamic parameter specification
    typedef sqlstate<DAL_SQLSTATE_07001, SQLSTATE_07000>         SQLSTATE_07001;
    typedef sqlstate<DAL_SQLSTATE_07001, SQLSTATE_07000>         SQLSTATE_using_clause_does_not_match_dynamic_parameter_specification;

// using clause does not match target specification
    typedef sqlstate<DAL_SQLSTATE_07002, SQLSTATE_07000>         SQLSTATE_07002;
    typedef sqlstate<DAL_SQLSTATE_07002, SQLSTATE_07000>         SQLSTATE_using_clause_does_not_match_target_specification;

// cursor specification cannot be executed
    typedef sqlstate<DAL_SQLSTATE_07003, SQLSTATE_07000>         SQLSTATE_07003;
    typedef sqlstate<DAL_SQLSTATE_07003, SQLSTATE_07000>         SQLSTATE_cursor_specification_cannot_be_executed;

// using clause required for dynamic parameters
    typedef sqlstate<DAL_SQLSTATE_07004, SQLSTATE_07000>         SQLSTATE_07004;
    typedef sqlstate<DAL_SQLSTATE_07004, SQLSTATE_07000>         SQLSTATE_using_clause_required_for_dynamic_parameters;

// prepared statement not a cursor specification
    typedef sqlstate<DAL_SQLSTATE_07005, SQLSTATE_07000>         SQLSTATE_07005;
    typedef sqlstate<DAL_SQLSTATE_07005, SQLSTATE_07000>         SQLSTATE_prepared_statement_not_a_cursor_specification;

// restricted data type attribute violation
    typedef sqlstate<DAL_SQLSTATE_07006, SQLSTATE_07000>         SQLSTATE_07006;
    typedef sqlstate<DAL_SQLSTATE_07006, SQLSTATE_07000>         SQLSTATE_restricted_data_type_attribute_violation;

// using clause required for result fields
    typedef sqlstate<DAL_SQLSTATE_07007, SQLSTATE_07000>         SQLSTATE_07007;
    typedef sqlstate<DAL_SQLSTATE_07007, SQLSTATE_07000>         SQLSTATE_using_clause_required_for_result_fields;

// invalid descriptor count
    typedef sqlstate<DAL_SQLSTATE_07008, SQLSTATE_07000>         SQLSTATE_07008;
    typedef sqlstate<DAL_SQLSTATE_07008, SQLSTATE_07000>         SQLSTATE_invalid_descriptor_count;

// invalid descriptor index
    typedef sqlstate<DAL_SQLSTATE_07009, SQLSTATE_07000>         SQLSTATE_07009;
    typedef sqlstate<DAL_SQLSTATE_07009, SQLSTATE_07000>         SQLSTATE_invalid_descriptor_index;

// data type transform function violation
    typedef sqlstate<DAL_SQLSTATE_0700B, SQLSTATE_07000>         SQLSTATE_0700B;
    typedef sqlstate<DAL_SQLSTATE_0700B, SQLSTATE_07000>         SQLSTATE_data_type_transform_function_violation;

// undefined DATA value
    typedef sqlstate<DAL_SQLSTATE_0700C, SQLSTATE_07000>         SQLSTATE_0700C;
    typedef sqlstate<DAL_SQLSTATE_0700C, SQLSTATE_07000>         SQLSTATE_undefined_data_value;

// invalid DATA target
    typedef sqlstate<DAL_SQLSTATE_0700D, SQLSTATE_07000>         SQLSTATE_0700D;
    typedef sqlstate<DAL_SQLSTATE_0700D, SQLSTATE_07000>         SQLSTATE_invalid_data_target;

// invalid LEVEL value
    typedef sqlstate<DAL_SQLSTATE_0700E, SQLSTATE_07000>         SQLSTATE_0700E;
    typedef sqlstate<DAL_SQLSTATE_0700E, SQLSTATE_07000>         SQLSTATE_invalid_level_value;

// invalid DATETIME_INTERVAL_CODE
    typedef sqlstate<DAL_SQLSTATE_0700F, SQLSTATE_07000>         SQLSTATE_0700F;
    typedef sqlstate<DAL_SQLSTATE_0700F, SQLSTATE_07000>         SQLSTATE_invalid_datetime_interval_code;

// connection exception
    typedef sqlstate<DAL_SQLSTATE_08000>                         SQLSTATE_08000;
    typedef sqlstate<DAL_SQLSTATE_08000>                         SQLSTATE_connection_exception;

// SQL-client unable to establish SQL-connection
    typedef sqlstate<DAL_SQLSTATE_08001, SQLSTATE_08000>         SQLSTATE_08001;
    typedef sqlstate<DAL_SQLSTATE_08001, SQLSTATE_08000>         SQLSTATE_sql_client_unable_to_establish_sql_connection;

// connection name in use
    typedef sqlstate<DAL_SQLSTATE_08002, SQLSTATE_08000>         SQLSTATE_08002;
    typedef sqlstate<DAL_SQLSTATE_08002, SQLSTATE_08000>         SQLSTATE_connection_name_in_use;

// connection does not exist
    typedef sqlstate<DAL_SQLSTATE_08003, SQLSTATE_08000>         SQLSTATE_08003;
    typedef sqlstate<DAL_SQLSTATE_08003, SQLSTATE_08000>         SQLSTATE_connection_does_not_exist;

// SQL-server rejected establishment of SQL-connection
    typedef sqlstate<DAL_SQLSTATE_08004, SQLSTATE_08000>         SQLSTATE_08004;
    typedef sqlstate<DAL_SQLSTATE_08004, SQLSTATE_08000>         SQLSTATE_sql_server_rejected_establishment_of_sql_connection;

// connection failure
    typedef sqlstate<DAL_SQLSTATE_08006, SQLSTATE_08000>         SQLSTATE_08006;
    typedef sqlstate<DAL_SQLSTATE_08006, SQLSTATE_08000>         SQLSTATE_connection_failure;

// transaction resolution unknown
    typedef sqlstate<DAL_SQLSTATE_08007, SQLSTATE_08000>         SQLSTATE_08007;
    typedef sqlstate<DAL_SQLSTATE_08007, SQLSTATE_08000>         SQLSTATE_transaction_resolution_unknown;

// trigger action exception
    typedef sqlstate<DAL_SQLSTATE_09000>                         SQLSTATE_09000;
    typedef sqlstate<DAL_SQLSTATE_09000>                         SQLSTATE_trigger_action_exception;

// feature not supported
    typedef sqlstate<DAL_SQLSTATE_0A000>                         SQLSTATE_0A000;
    typedef sqlstate<DAL_SQLSTATE_0A000>                         SQLSTATE_feature_not_supported;

// multiple server transactions
    typedef sqlstate<DAL_SQLSTATE_0A001, SQLSTATE_0A000>         SQLSTATE_0A001;
    typedef sqlstate<DAL_SQLSTATE_0A001, SQLSTATE_0A000>         SQLSTATE_multiple_server_transactions;

// invalid target type specification
    typedef sqlstate<DAL_SQLSTATE_0D000>                         SQLSTATE_0D000;
    typedef sqlstate<DAL_SQLSTATE_0D000>                         SQLSTATE_invalid_target_type_specification;

// invalid schema name list specification
    typedef sqlstate<DAL_SQLSTATE_0E000>                         SQLSTATE_0E000;
    typedef sqlstate<DAL_SQLSTATE_0E000>                         SQLSTATE_invalid_schema_name_list_specification;

// locator exception
    typedef sqlstate<DAL_SQLSTATE_0F000>                         SQLSTATE_0F000;
    typedef sqlstate<DAL_SQLSTATE_0F000>                         SQLSTATE_locator_exception;

// invalid specification
    typedef sqlstate<DAL_SQLSTATE_0F001, SQLSTATE_0F000>         SQLSTATE_0F001;
    typedef sqlstate<DAL_SQLSTATE_0F001, SQLSTATE_0F000>         SQLSTATE_invalid_specification;

// invalid grantor
    typedef sqlstate<DAL_SQLSTATE_0L000>                         SQLSTATE_0L000;
    typedef sqlstate<DAL_SQLSTATE_0L000>                         SQLSTATE_invalid_grantor;

// invalid SQL-invoked procedure reference
    typedef sqlstate<DAL_SQLSTATE_0M000>                         SQLSTATE_0M000;
    typedef sqlstate<DAL_SQLSTATE_0M000>                         SQLSTATE_invalid_sql_invoked_procedure_reference;

// invalid role specification
    typedef sqlstate<DAL_SQLSTATE_0P000>                         SQLSTATE_0P000;
    typedef sqlstate<DAL_SQLSTATE_0P000>                         SQLSTATE_invalid_role_specification;

// invalid transform group name specification
    typedef sqlstate<DAL_SQLSTATE_0S000>                         SQLSTATE_0S000;
    typedef sqlstate<DAL_SQLSTATE_0S000>                         SQLSTATE_invalid_transform_group_name_specification;

// target table disagrees with cursor specification
    typedef sqlstate<DAL_SQLSTATE_0T000>                         SQLSTATE_0T000;
    typedef sqlstate<DAL_SQLSTATE_0T000>                         SQLSTATE_target_table_disagrees_with_cursor_specification;

// attempt to assign to non-updatable column
    typedef sqlstate<DAL_SQLSTATE_0U000>                         SQLSTATE_0U000;
    typedef sqlstate<DAL_SQLSTATE_0U000>                         SQLSTATE_attempt_to_assign_to_non_updatable_column;

// attempt to assign to ordering column
    typedef sqlstate<DAL_SQLSTATE_0V000>                         SQLSTATE_0V000;
    typedef sqlstate<DAL_SQLSTATE_0V000>                         SQLSTATE_attemnt_to_assign_to_ordering_column;

// prohibited statement encountered during trigger execution
    typedef sqlstate<DAL_SQLSTATE_0W000>                         SQLSTATE_0W000;
    typedef sqlstate<DAL_SQLSTATE_0W000>                         SQLSTATE_prohibited_statement_encountered_during_trigger_execution;

// diagnostics exception
    typedef sqlstate<DAL_SQLSTATE_0Z000>                         SQLSTATE_0Z000;
    typedef sqlstate<DAL_SQLSTATE_0Z000>                         SQLSTATE_diagnostics_exception;

// maximum number of stacked diagnostics areas exceeded
    typedef sqlstate<DAL_SQLSTATE_0Z001, SQLSTATE_0Z000>         SQLSTATE_0Z001;
    typedef sqlstate<DAL_SQLSTATE_0Z001, SQLSTATE_0Z000>         SQLSTATE_maximum_number_of_stacked_diagnostics_areas_exceeded;

// cardinality violation
    typedef sqlstate<DAL_SQLSTATE_21000>                         SQLSTATE_21000;
    typedef sqlstate<DAL_SQLSTATE_21000>                         SQLSTATE_cardinality_violation;

// data exception
    typedef sqlstate<DAL_SQLSTATE_22000>                         SQLSTATE_22000;
    typedef sqlstate<DAL_SQLSTATE_22000>                         SQLSTATE_data_exception;

// string data, right truncation
    typedef sqlstate<DAL_SQLSTATE_22001, SQLSTATE_22000>         SQLSTATE_22001;
    typedef sqlstate<DAL_SQLSTATE_22001, SQLSTATE_22000>         SQLSTATE_string_data,_right_truncation;

// null value, no indicator parameter
    typedef sqlstate<DAL_SQLSTATE_22002, SQLSTATE_22000>         SQLSTATE_22002;
    typedef sqlstate<DAL_SQLSTATE_22002, SQLSTATE_22000>         SQLSTATE_null_value,_no_indicator_parameter;

// numeric value out of range
    typedef sqlstate<DAL_SQLSTATE_22003, SQLSTATE_22000>         SQLSTATE_22003;
    typedef sqlstate<DAL_SQLSTATE_22003, SQLSTATE_22000>         SQLSTATE_numeric_value_out_of_range;

// null value not allowed
    typedef sqlstate<DAL_SQLSTATE_22004, SQLSTATE_22000>         SQLSTATE_22004;
    typedef sqlstate<DAL_SQLSTATE_22004, SQLSTATE_22000>         SQLSTATE_null_value_not_allowed;

// error in assignment
    typedef sqlstate<DAL_SQLSTATE_22005, SQLSTATE_22000>         SQLSTATE_22005;
    typedef sqlstate<DAL_SQLSTATE_22005, SQLSTATE_22000>         SQLSTATE_error_in_assignment;

// invalid interval format
    typedef sqlstate<DAL_SQLSTATE_22006, SQLSTATE_22000>         SQLSTATE_22006;
    typedef sqlstate<DAL_SQLSTATE_22006, SQLSTATE_22000>         SQLSTATE_invalid_interval_format;

// invalid datetime format
    typedef sqlstate<DAL_SQLSTATE_22007, SQLSTATE_22000>         SQLSTATE_22007;
    typedef sqlstate<DAL_SQLSTATE_22007, SQLSTATE_22000>         SQLSTATE_invalid_datetime_format;

// datetime field overflow
    typedef sqlstate<DAL_SQLSTATE_22008, SQLSTATE_22000>         SQLSTATE_22008;
    typedef sqlstate<DAL_SQLSTATE_22008, SQLSTATE_22000>         SQLSTATE_datetime_field_overflow;

// invalid time zone displacement value
    typedef sqlstate<DAL_SQLSTATE_22009, SQLSTATE_22000>         SQLSTATE_22009;
    typedef sqlstate<DAL_SQLSTATE_22009, SQLSTATE_22000>         SQLSTATE_invalid_time_zone_displacement_value;

// escape character conflict
    typedef sqlstate<DAL_SQLSTATE_2200B, SQLSTATE_22000>         SQLSTATE_2200B;
    typedef sqlstate<DAL_SQLSTATE_2200B, SQLSTATE_22000>         SQLSTATE_escape_character_conflict;

// invalid use of escape character
    typedef sqlstate<DAL_SQLSTATE_2200C, SQLSTATE_22000>         SQLSTATE_2200C;
    typedef sqlstate<DAL_SQLSTATE_2200C, SQLSTATE_22000>         SQLSTATE_invalid_use_of_escape_character;

// invalid escape octet
    typedef sqlstate<DAL_SQLSTATE_2200D, SQLSTATE_22000>         SQLSTATE_2200D;
    typedef sqlstate<DAL_SQLSTATE_2200D, SQLSTATE_22000>         SQLSTATE_invalid_escape_octet;

// null value in array target
    typedef sqlstate<DAL_SQLSTATE_2200E, SQLSTATE_22000>         SQLSTATE_2200E;
    typedef sqlstate<DAL_SQLSTATE_2200E, SQLSTATE_22000>         SQLSTATE_null_value_in_array_target;

// zero-length character string
    typedef sqlstate<DAL_SQLSTATE_2200F, SQLSTATE_22000>         SQLSTATE_2200F;
    typedef sqlstate<DAL_SQLSTATE_2200F, SQLSTATE_22000>         SQLSTATE_zero_length_character_string;

// most specific type mismatch
    typedef sqlstate<DAL_SQLSTATE_2200G, SQLSTATE_22000>         SQLSTATE_2200G;
    typedef sqlstate<DAL_SQLSTATE_2200G, SQLSTATE_22000>         SQLSTATE_most_specific_type_mismatch;

// sequence generator limit exceeded
    typedef sqlstate<DAL_SQLSTATE_2200H, SQLSTATE_22000>         SQLSTATE_2200H;
    typedef sqlstate<DAL_SQLSTATE_2200H, SQLSTATE_22000>         SQLSTATE_sequence_generator_limit_exceeded;

// interval value out of range
    typedef sqlstate<DAL_SQLSTATE_2200P, SQLSTATE_22000>         SQLSTATE_2200P;
    typedef sqlstate<DAL_SQLSTATE_2200P, SQLSTATE_22000>         SQLSTATE_interval_value_out_of_range;

// multiset value overflow
    typedef sqlstate<DAL_SQLSTATE_2200Q, SQLSTATE_22000>         SQLSTATE_2200Q;
    typedef sqlstate<DAL_SQLSTATE_2200Q, SQLSTATE_22000>         SQLSTATE_multiset_value_overflow;

// invalid indicator parameter value
    typedef sqlstate<DAL_SQLSTATE_22010, SQLSTATE_22000>         SQLSTATE_22010;
    typedef sqlstate<DAL_SQLSTATE_22010, SQLSTATE_22000>         SQLSTATE_invalid_indicator_parameter_value;

// substring error
    typedef sqlstate<DAL_SQLSTATE_22011, SQLSTATE_22000>         SQLSTATE_22011;
    typedef sqlstate<DAL_SQLSTATE_22011, SQLSTATE_22000>         SQLSTATE_substring_error;

// division by zero
    typedef sqlstate<DAL_SQLSTATE_22012, SQLSTATE_22000>         SQLSTATE_22012;
    typedef sqlstate<DAL_SQLSTATE_22012, SQLSTATE_22000>         SQLSTATE_division_by_zero;

// invalid preceding or following size in window function
    typedef sqlstate<DAL_SQLSTATE_22013, SQLSTATE_22000>         SQLSTATE_22013;
    typedef sqlstate<DAL_SQLSTATE_22013, SQLSTATE_22000>         SQLSTATE_invalid_preceding_or_following_size_in_window_function;

// invalid argument for NTILE function
    typedef sqlstate<DAL_SQLSTATE_22014, SQLSTATE_22000>         SQLSTATE_22014;
    typedef sqlstate<DAL_SQLSTATE_22014, SQLSTATE_22000>         SQLSTATE_invalid_argument_for_ntile_function;

// interval field overflow
    typedef sqlstate<DAL_SQLSTATE_22015, SQLSTATE_22000>         SQLSTATE_22015;
    typedef sqlstate<DAL_SQLSTATE_22015, SQLSTATE_22000>         SQLSTATE_interval_field_overflow;

// invalid argument for NTH_VALUE function
    typedef sqlstate<DAL_SQLSTATE_22016, SQLSTATE_22000>         SQLSTATE_22016;
    typedef sqlstate<DAL_SQLSTATE_22016, SQLSTATE_22000>         SQLSTATE_invalid_argument_for_nth_value_function;

// invalid character value for cast
    typedef sqlstate<DAL_SQLSTATE_22018, SQLSTATE_22000>         SQLSTATE_22018;
    typedef sqlstate<DAL_SQLSTATE_22018, SQLSTATE_22000>         SQLSTATE_invalid_character_value_for_cast;

// invalid escape character
    typedef sqlstate<DAL_SQLSTATE_22019, SQLSTATE_22000>         SQLSTATE_22019;
    typedef sqlstate<DAL_SQLSTATE_22019, SQLSTATE_22000>         SQLSTATE_invalid_escape_character;

// invalid regular expression
    typedef sqlstate<DAL_SQLSTATE_2201B, SQLSTATE_22000>         SQLSTATE_2201B;
    typedef sqlstate<DAL_SQLSTATE_2201B, SQLSTATE_22000>         SQLSTATE_invalid_regular_expression;

// null row not permitted in table
    typedef sqlstate<DAL_SQLSTATE_2201C, SQLSTATE_22000>         SQLSTATE_2201C;
    typedef sqlstate<DAL_SQLSTATE_2201C, SQLSTATE_22000>         SQLSTATE_null_row_not_permitted_in_table;

// invalid argument for natural logarithm
    typedef sqlstate<DAL_SQLSTATE_2201E, SQLSTATE_22000>         SQLSTATE_2201E;
    typedef sqlstate<DAL_SQLSTATE_2201E, SQLSTATE_22000>         SQLSTATE_invalid_argument_for_natural_logarithm;

// invalid argument for power function
    typedef sqlstate<DAL_SQLSTATE_2201F, SQLSTATE_22000>         SQLSTATE_2201F;
    typedef sqlstate<DAL_SQLSTATE_2201F, SQLSTATE_22000>         SQLSTATE_invalid_argument_for_power_function;

// invalid argument for width bucket function
    typedef sqlstate<DAL_SQLSTATE_2201G, SQLSTATE_22000>         SQLSTATE_2201G;
    typedef sqlstate<DAL_SQLSTATE_2201G, SQLSTATE_22000>         SQLSTATE_invalid_argument_for_width_bucket_function;

// invalid XQuery regular expression
    typedef sqlstate<DAL_SQLSTATE_2201S, SQLSTATE_22000>         SQLSTATE_2201S;
    typedef sqlstate<DAL_SQLSTATE_2201S, SQLSTATE_22000>         SQLSTATE_invalid_xquery_regular_expression;

// invalid XQuery option flag
    typedef sqlstate<DAL_SQLSTATE_2201T, SQLSTATE_22000>         SQLSTATE_2201T;
    typedef sqlstate<DAL_SQLSTATE_2201T, SQLSTATE_22000>         SQLSTATE_invalid_xquery_option_flag;

// attempt to replace a zero-length string
    typedef sqlstate<DAL_SQLSTATE_2201U, SQLSTATE_22000>         SQLSTATE_2201U;
    typedef sqlstate<DAL_SQLSTATE_2201U, SQLSTATE_22000>         SQLSTATE_attempt_to_replace_a_zero_length_string;

// invalid XQuery replacement string
    typedef sqlstate<DAL_SQLSTATE_2201V, SQLSTATE_22000>         SQLSTATE_2201V;
    typedef sqlstate<DAL_SQLSTATE_2201V, SQLSTATE_22000>         SQLSTATE_invalid_xquery_replacement_string;

// invalid row count in fetch first clause
    typedef sqlstate<DAL_SQLSTATE_2201W, SQLSTATE_22000>         SQLSTATE_2201W;
    typedef sqlstate<DAL_SQLSTATE_2201W, SQLSTATE_22000>         SQLSTATE_invalid_row_count_in_fetch_first_clause;

// invalid row count in result offset clause
    typedef sqlstate<DAL_SQLSTATE_2201X, SQLSTATE_22000>         SQLSTATE_2201X;
    typedef sqlstate<DAL_SQLSTATE_2201X, SQLSTATE_22000>         SQLSTATE_invalid_row_count_in_result_offset_clause;

// character not in repertoire
    typedef sqlstate<DAL_SQLSTATE_22021, SQLSTATE_22000>         SQLSTATE_22021;
    typedef sqlstate<DAL_SQLSTATE_22021, SQLSTATE_22000>         SQLSTATE_character_not_in_repertoire;

// indicator overflow
    typedef sqlstate<DAL_SQLSTATE_22022, SQLSTATE_22000>         SQLSTATE_22022;
    typedef sqlstate<DAL_SQLSTATE_22022, SQLSTATE_22000>         SQLSTATE_indicator_overflow;

// invalid parameter value
    typedef sqlstate<DAL_SQLSTATE_22023, SQLSTATE_22000>         SQLSTATE_22023;
    typedef sqlstate<DAL_SQLSTATE_22023, SQLSTATE_22000>         SQLSTATE_invalid_parameter_value;

// unterminated C string
    typedef sqlstate<DAL_SQLSTATE_22024, SQLSTATE_22000>         SQLSTATE_22024;
    typedef sqlstate<DAL_SQLSTATE_22024, SQLSTATE_22000>         SQLSTATE_unterminated_c_string;

// invalid escape sequence
    typedef sqlstate<DAL_SQLSTATE_22025, SQLSTATE_22000>         SQLSTATE_22025;
    typedef sqlstate<DAL_SQLSTATE_22025, SQLSTATE_22000>         SQLSTATE_invalid_escape_sequence;

// string data, length mismatch
    typedef sqlstate<DAL_SQLSTATE_22026, SQLSTATE_22000>         SQLSTATE_22026;
    typedef sqlstate<DAL_SQLSTATE_22026, SQLSTATE_22000>         SQLSTATE_string_data_length_mismatch;

// trim error
    typedef sqlstate<DAL_SQLSTATE_22027, SQLSTATE_22000>         SQLSTATE_22027;
    typedef sqlstate<DAL_SQLSTATE_22027, SQLSTATE_22000>         SQLSTATE_trim_error;

// noncharacter in UCS string
    typedef sqlstate<DAL_SQLSTATE_22029, SQLSTATE_22000>         SQLSTATE_22029;
    typedef sqlstate<DAL_SQLSTATE_22029, SQLSTATE_22000>         SQLSTATE_noncharacter_in_ucs_string;

// null value substitued for mutator subject parameter
    typedef sqlstate<DAL_SQLSTATE_2202D, SQLSTATE_22000>         SQLSTATE_2202D;
    typedef sqlstate<DAL_SQLSTATE_2202D, SQLSTATE_22000>         SQLSTATE_null_value_substitued_for_mutator_subject_parameter;

// array element error
    typedef sqlstate<DAL_SQLSTATE_2202E, SQLSTATE_22000>         SQLSTATE_2202E;
    typedef sqlstate<DAL_SQLSTATE_2202E, SQLSTATE_22000>         SQLSTATE_array_element_error;

// array data, right truncation
    typedef sqlstate<DAL_SQLSTATE_2202F, SQLSTATE_22000>         SQLSTATE_2202F;
    typedef sqlstate<DAL_SQLSTATE_2202F, SQLSTATE_22000>         SQLSTATE_array_data_right_truncation;

// invalid repeat argument in a sample clause
    typedef sqlstate<DAL_SQLSTATE_2202G, SQLSTATE_22000>         SQLSTATE_2202G;
    typedef sqlstate<DAL_SQLSTATE_2202G, SQLSTATE_22000>         SQLSTATE_invalid_repeat_argument_in_a_sample_clause;

// invalid sample size
    typedef sqlstate<DAL_SQLSTATE_2202H, SQLSTATE_22000>         SQLSTATE_2202H;
    typedef sqlstate<DAL_SQLSTATE_2202H, SQLSTATE_22000>         SQLSTATE_invalid_sample_size;

// integrity constraint violation
    typedef sqlstate<DAL_SQLSTATE_23000>                         SQLSTATE_23000;
    typedef sqlstate<DAL_SQLSTATE_23000>                         SQLSTATE_integrity_constraint_violation;

// restrict violation
    typedef sqlstate<DAL_SQLSTATE_23001, SQLSTATE_23000>         SQLSTATE_23001;
    typedef sqlstate<DAL_SQLSTATE_23001, SQLSTATE_23000>         SQLSTATE_restrict_violation;

// invalid cursor state
    typedef sqlstate<DAL_SQLSTATE_24000>                         SQLSTATE_24000;
    typedef sqlstate<DAL_SQLSTATE_24000>                         SQLSTATE_invalid_cursor_state;

// invalid transaction state
    typedef sqlstate<DAL_SQLSTATE_25000>                         SQLSTATE_25000;
    typedef sqlstate<DAL_SQLSTATE_25000>                         SQLSTATE_invalid_transaction_state;

// active SQL-transaction
    typedef sqlstate<DAL_SQLSTATE_25001, SQLSTATE_25000>         SQLSTATE_25001;
    typedef sqlstate<DAL_SQLSTATE_25001, SQLSTATE_25000>         SQLSTATE_active_sql_transaction;

// branch transaction already active
    typedef sqlstate<DAL_SQLSTATE_25002, SQLSTATE_25000>         SQLSTATE_25002;
    typedef sqlstate<DAL_SQLSTATE_25002, SQLSTATE_25000>         SQLSTATE_branch_transaction_already_active;

// inappropriate access mode for branch transaction
    typedef sqlstate<DAL_SQLSTATE_25003, SQLSTATE_25000>         SQLSTATE_25003;
    typedef sqlstate<DAL_SQLSTATE_25003, SQLSTATE_25000>         SQLSTATE_inappropriate_access_mode_for_branch_transaction;

// inappropriate isolation level for branch transaction
    typedef sqlstate<DAL_SQLSTATE_25004, SQLSTATE_25000>         SQLSTATE_25004;
    typedef sqlstate<DAL_SQLSTATE_25004, SQLSTATE_25000>         SQLSTATE_inappropriate_isolation_level_for_branch_transaction;

// no active SQL-transaction for branch transaction
    typedef sqlstate<DAL_SQLSTATE_25005, SQLSTATE_25000>         SQLSTATE_25005;
    typedef sqlstate<DAL_SQLSTATE_25005, SQLSTATE_25000>         SQLSTATE_no_active_sql_transaction_for_branch_transaction;

// read-only SQL-transaction
    typedef sqlstate<DAL_SQLSTATE_25006, SQLSTATE_25000>         SQLSTATE_25006;
    typedef sqlstate<DAL_SQLSTATE_25006, SQLSTATE_25000>         SQLSTATE_read_only_sql_transaction;

// schema and data statement mixing not supported
    typedef sqlstate<DAL_SQLSTATE_25007, SQLSTATE_25000>         SQLSTATE_25007;
    typedef sqlstate<DAL_SQLSTATE_25007, SQLSTATE_25000>         SQLSTATE_schema_and_data_statement_mixing_not_supported;

// held cursor requires same isolation level
    typedef sqlstate<DAL_SQLSTATE_25008, SQLSTATE_25000>         SQLSTATE_25008;
    typedef sqlstate<DAL_SQLSTATE_25008, SQLSTATE_25000>         SQLSTATE_held_cursor_requires_same_isolation_level;

// invalid SQL statement name
    typedef sqlstate<DAL_SQLSTATE_26000>                         SQLSTATE_26000;
    typedef sqlstate<DAL_SQLSTATE_26000>                         SQLSTATE_invalid_sql_statement_name;

// riggered data change violation
    typedef sqlstate<DAL_SQLSTATE_27000>                         SQLSTATE_27000;
    typedef sqlstate<DAL_SQLSTATE_27000>                         SQLSTATE_riggered_data_change_violation;

// invalid authorization specification
    typedef sqlstate<DAL_SQLSTATE_28000>                         SQLSTATE_28000;
    typedef sqlstate<DAL_SQLSTATE_28000>                         SQLSTATE_invalid_authorization_specification;

// dependent privilege descriptors still exist
    typedef sqlstate<DAL_SQLSTATE_2B000>                         SQLSTATE_2B000;
    typedef sqlstate<DAL_SQLSTATE_2B000>                         SQLSTATE_dependent_privilege_descriptors_still_exist;

// invalid character set name
    typedef sqlstate<DAL_SQLSTATE_2C000>                         SQLSTATE_2C000;
    typedef sqlstate<DAL_SQLSTATE_2C000>                         SQLSTATE_invalid_character_set_name;

// invalid transaction termination
    typedef sqlstate<DAL_SQLSTATE_2D000>                         SQLSTATE_2D000;
    typedef sqlstate<DAL_SQLSTATE_2D000>                         SQLSTATE_invalid_transaction_termination;

// invalid connection name
    typedef sqlstate<DAL_SQLSTATE_2E000>                         SQLSTATE_2E000;
    typedef sqlstate<DAL_SQLSTATE_2E000>                         SQLSTATE_invalid_connection_name;

// SQL routine exception
    typedef sqlstate<DAL_SQLSTATE_2F000>                         SQLSTATE_2F000;
    typedef sqlstate<DAL_SQLSTATE_2F000>                         SQLSTATE_sql_routine_exception;

// modifying SQL-data not permitted
    typedef sqlstate<DAL_SQLSTATE_2F002, SQLSTATE_2F000>         SQLSTATE_2F002;
    typedef sqlstate<DAL_SQLSTATE_2F002, SQLSTATE_2F000>         SQLSTATE_modifying_sql_data_not_permitted;

// prohibited SQL-statement attempted
    typedef sqlstate<DAL_SQLSTATE_2F003, SQLSTATE_2F000>         SQLSTATE_2F003;
    typedef sqlstate<DAL_SQLSTATE_2F003, SQLSTATE_2F000>         SQLSTATE_prohibited_sql_statement_attempted;

// reading SQL-data not permitted
    typedef sqlstate<DAL_SQLSTATE_2F004, SQLSTATE_2F000>         SQLSTATE_2F004;
    typedef sqlstate<DAL_SQLSTATE_2F004, SQLSTATE_2F000>         SQLSTATE_reading_sql_data_not_permitted;

// function executed no return statement
    typedef sqlstate<DAL_SQLSTATE_2F005, SQLSTATE_2F000>         SQLSTATE_2F005;
    typedef sqlstate<DAL_SQLSTATE_2F005, SQLSTATE_2F000>         SQLSTATE_function_executed_no_return_statement;

// invalid collation name
    typedef sqlstate<DAL_SQLSTATE_2H000>                         SQLSTATE_2H000;
    typedef sqlstate<DAL_SQLSTATE_2H000>                         SQLSTATE_invalid_collation_name;

// invalid SQL statement identifier
    typedef sqlstate<DAL_SQLSTATE_30000>                         SQLSTATE_30000;
    typedef sqlstate<DAL_SQLSTATE_30000>                         SQLSTATE_invalid_sql_statement_identifier;

// invalid SQL descriptor name
    typedef sqlstate<DAL_SQLSTATE_33000>                         SQLSTATE_33000;
    typedef sqlstate<DAL_SQLSTATE_33000>                         SQLSTATE_invalid_sql_descriptor_name;

// invalid cursor name
    typedef sqlstate<DAL_SQLSTATE_34000>                         SQLSTATE_34000;
    typedef sqlstate<DAL_SQLSTATE_34000>                         SQLSTATE_invalid_cursor_name;

// invalid condition number
    typedef sqlstate<DAL_SQLSTATE_35000>                         SQLSTATE_35000;
    typedef sqlstate<DAL_SQLSTATE_35000>                         SQLSTATE_invalid_condition_number;

// cursor sensitivity exception
    typedef sqlstate<DAL_SQLSTATE_36000>                         SQLSTATE_36000;
    typedef sqlstate<DAL_SQLSTATE_36000>                         SQLSTATE_cursor_sensitivity_exception;

// request rejected
    typedef sqlstate<DAL_SQLSTATE_36001, SQLSTATE_36000>         SQLSTATE_36001;
    typedef sqlstate<DAL_SQLSTATE_36001, SQLSTATE_36000>         SQLSTATE_request_rejected;

// request failed
    typedef sqlstate<DAL_SQLSTATE_36002, SQLSTATE_36000>         SQLSTATE_36002;
    typedef sqlstate<DAL_SQLSTATE_36002, SQLSTATE_36000>         SQLSTATE_request_failed;

// external routine exception
    typedef sqlstate<DAL_SQLSTATE_38000>                         SQLSTATE_38000;
    typedef sqlstate<DAL_SQLSTATE_38000>                         SQLSTATE_external_routine_exception;

// containg SQL not permitted
    typedef sqlstate<DAL_SQLSTATE_38001, SQLSTATE_38000>         SQLSTATE_38001;
    typedef sqlstate<DAL_SQLSTATE_38001, SQLSTATE_38000>         SQLSTATE_external_containg_sql_not_permitted;

// modifying SQL-data not permitted
    typedef sqlstate<DAL_SQLSTATE_38002, SQLSTATE_38000>         SQLSTATE_38002;
    typedef sqlstate<DAL_SQLSTATE_38002, SQLSTATE_38000>         SQLSTATE_external_modifying_sql_data_not_permitted;

// prohibited SQL-statement attempted
    typedef sqlstate<DAL_SQLSTATE_38003, SQLSTATE_38000>         SQLSTATE_38003;
    typedef sqlstate<DAL_SQLSTATE_38003, SQLSTATE_38000>         SQLSTATE_external_prohibited_sql_statement_attempted;

// reading SQL-data not permitted
    typedef sqlstate<DAL_SQLSTATE_38004, SQLSTATE_38000>         SQLSTATE_38004;
    typedef sqlstate<DAL_SQLSTATE_38004, SQLSTATE_38000>         SQLSTATE_external_reading_sql_data_not_permitted;

// external routine invocation exception
    typedef sqlstate<DAL_SQLSTATE_39000>                         SQLSTATE_39000;
    typedef sqlstate<DAL_SQLSTATE_39000>                         SQLSTATE_external_routine_invocation_exception;

// null value not allowed
    typedef sqlstate<DAL_SQLSTATE_39004, SQLSTATE_39000>         SQLSTATE_39004;
    typedef sqlstate<DAL_SQLSTATE_39004, SQLSTATE_39000>         SQLSTATE_routine_null_value_not_allowed;

// savepoint exception
    typedef sqlstate<DAL_SQLSTATE_3B000>                         SQLSTATE_3B000;
    typedef sqlstate<DAL_SQLSTATE_3B000>                         SQLSTATE_savepoint_exception;

// invalid specification
    typedef sqlstate<DAL_SQLSTATE_3B001, SQLSTATE_3B000>         SQLSTATE_3B001;
    typedef sqlstate<DAL_SQLSTATE_3B001, SQLSTATE_3B000>         SQLSTATE_invalid_savepoint_specification;

// too many savepoints
    typedef sqlstate<DAL_SQLSTATE_3B002, SQLSTATE_3B000>         SQLSTATE_3B002;
    typedef sqlstate<DAL_SQLSTATE_3B002, SQLSTATE_3B000>         SQLSTATE_too_many_savepoints;

// ambigous cursor name
    typedef sqlstate<DAL_SQLSTATE_3C000>                         SQLSTATE_3C000;
    typedef sqlstate<DAL_SQLSTATE_3C000>                         SQLSTATE_ambigous_cursor_name;

// invalid catalog name
    typedef sqlstate<DAL_SQLSTATE_3D000>                         SQLSTATE_3D000;
    typedef sqlstate<DAL_SQLSTATE_3D000>                         SQLSTATE_invalid_catalog_name;

// invalid schema name
    typedef sqlstate<DAL_SQLSTATE_3F000>                         SQLSTATE_3F000;
    typedef sqlstate<DAL_SQLSTATE_3F000>                         SQLSTATE_invalid_schema_name;

// syntax error or access rule violation
    typedef sqlstate<DAL_SQLSTATE_42000>                         SQLSTATE_42000;
    typedef sqlstate<DAL_SQLSTATE_42000>                         SQLSTATE_syntax_error_or_access_rule_violation;

// with check option violation
    typedef sqlstate<DAL_SQLSTATE_44000>                         SQLSTATE_44000;
    typedef sqlstate<DAL_SQLSTATE_44000>                         SQLSTATE_with_check_option_violation;

// transaction rollback
    typedef sqlstate<DAL_SQLSTATE_50000>                         SQLSTATE_50000;
    typedef sqlstate<DAL_SQLSTATE_50000>                         SQLSTATE_transaction_rollback;

// serialization failure
    typedef sqlstate<DAL_SQLSTATE_50001, SQLSTATE_50000>         SQLSTATE_50001;
    typedef sqlstate<DAL_SQLSTATE_50001, SQLSTATE_50000>         SQLSTATE_serialization_failure;

// integrity constraint violation
    typedef sqlstate<DAL_SQLSTATE_50002, SQLSTATE_50000>         SQLSTATE_50002;
    typedef sqlstate<DAL_SQLSTATE_50002, SQLSTATE_50000>         SQLSTATE_integrity_constraint_violation_rollback;

// statement completion unknown
    typedef sqlstate<DAL_SQLSTATE_50003, SQLSTATE_50000>         SQLSTATE_50003;
    typedef sqlstate<DAL_SQLSTATE_50003, SQLSTATE_50000>         SQLSTATE_statement_completion_unknown;

// triggered action exception
    typedef sqlstate<DAL_SQLSTATE_50004, SQLSTATE_50000>         SQLSTATE_50004;
    typedef sqlstate<DAL_SQLSTATE_50004, SQLSTATE_50000>         SQLSTATE_triggered_action_exception;


};




//------------------------------------------------------------------------------
/// @brief Main generic interface class
struct generic
{
    typedef IDbc                DBC;
    typedef IResult             RESULT;
    typedef IStmt               STMT;
    typedef IEnv                ENV;
    typedef Variant             VALUE;
    typedef generic_sqlstates   STATES;
    typedef IDiagnostic         DIAG;
    typedef ITable              TABLE;
    typedef IColumnDesc         COLUMNDESC;
//  typedef SqliteTypeInfo      TYPEINFO;


    ///
    /// @brief Create a new Environment
    ///
    /// Current supported drivers are:
    ///  - libsqlite
    ///
    DBWTL_EXPORT static ENV* createEnv(String driver); 
};


DAL_NAMESPACE_END




DB_NAMESPACE_BEGIN


template<typename tag>
struct db_traits<dal::generic, tag>
{
    typedef Environment<dal::generic, tag>      environment_type;
    typedef Connection<dal::generic, tag>       connection_type;
    typedef Statement<dal::generic, tag>        statement_type;
    typedef Result<dal::generic, tag>           resultset_type;
    typedef CachedResult<dal::generic, tag>     cached_resultset_type;
    typedef dal::generic::VALUE                 value_type;

    typedef dal::generic::RESULT                dal_resultset_type;
    typedef dal::generic::STMT                  dal_stmt_type;
    typedef dal::generic::ENV                   dal_env_type;
    typedef dal::generic::DBC                   dal_dbc_type;
    typedef dal::generic::DIAG                  dal_diag_type;

    typedef dal::generic::COLUMNDESC            dal_columndesc_type;
    typedef dal::generic::STATES                sqlstate_types;
    typedef basic_datatypes                     datatype_types;

    typedef dal::Variant                        dal_variant_type;

    enum { DB_SYSTEM_ID = 0 };
};


DB_NAMESPACE_END


#endif


//
// Local Variables:
// mode: C++
// c-file-style: "bsd"
// c-basic-offset: 4
// indent-tabs-mode: nil
// End:
//
