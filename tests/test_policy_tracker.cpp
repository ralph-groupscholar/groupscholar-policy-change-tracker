#include "../src/policy_tracker.h"

#include <cassert>
#include <iostream>

int main() {
  PolicyChangeInput input;
  input.title = "Policy";
  input.category = "Compliance";
  input.impact_level = "High";
  input.effective_date = "2026-02-01";
  input.owner = "Ops";
  input.notes = "Note";

  auto sql = build_insert_sql(input);
  assert(sql.find("insert into") != std::string::npos);
  assert(sql.find("Policy") != std::string::npos);

  auto schema = build_schema_sql();
  assert(schema.find("create schema") != std::string::npos);

  auto seed = build_seed_sql();
  assert(seed.find("policy_changes") != std::string::npos);

  auto select_sql = build_select_by_id_sql(3);
  assert(select_sql.find("where id = 3") != std::string::npos);

  QueryFilters filters;
  filters.category = "Compliance";
  filters.impact_level = "High";
  filters.since_date = "2026-01-01";
  auto where = build_where_clause(filters, {});
  assert(where.find("category = 'Compliance'") != std::string::npos);
  assert(where.find("impact_level = 'High'") != std::string::npos);
  assert(where.find("effective_date >= '2026-01-01'") != std::string::npos);

  auto group_col = resolve_report_group_column("impact");
  assert(group_col == "impact_level");

  assert(escape_csv("plain") == "plain");
  assert(escape_csv("needs,comma") == "\"needs,comma\"");
  assert(escape_csv("quote\"here") == "\"quote\"\"here\"");

  std::cout << "All tests passed.\n";
  return 0;
}
