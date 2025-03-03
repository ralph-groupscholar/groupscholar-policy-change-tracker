#ifndef GROUPSCHOLAR_POLICY_CHANGE_TRACKER_H
#define GROUPSCHOLAR_POLICY_CHANGE_TRACKER_H

#include <string>
#include <vector>

struct PolicyChangeInput {
  std::string title;
  std::string category;
  std::string impact_level;
  std::string effective_date;
  std::string owner;
  std::string notes;
};

struct ReportRow {
  std::string category;
  int count;
};

std::string sanitize_for_sql(const std::string &value);
std::string require_non_empty(const std::string &label, const std::string &value);
std::string build_insert_sql(const PolicyChangeInput &input);
std::string build_schema_sql();
std::string build_seed_sql();
std::vector<ReportRow> parse_report_rows(const std::string &raw);

#endif
