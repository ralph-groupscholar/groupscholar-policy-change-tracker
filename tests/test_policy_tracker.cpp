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

  std::cout << "All tests passed.\n";
  return 0;
}
