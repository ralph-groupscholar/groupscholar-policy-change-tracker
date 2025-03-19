# Group Scholar Policy Change Tracker

Command-line tool for logging scholarship policy changes, auditing impact, and generating quick summaries for the Group Scholar team.

## Features
- Create and store policy change entries in PostgreSQL
- List recent changes with effective dates, owners, and impact level
- Show full details (including notes) for a single change
- Summarize changes by category, impact, or owner for weekly reporting
- Filter lists and reports by date range, category, impact, or owner
- Track upcoming effective dates within a configurable window
- Seed the database with realistic sample entries

## Tech
- C++17
- libpq (PostgreSQL client library)

## Setup
1. Install libpq:
   ```sh
   brew install libpq
   ```
2. Build:
   ```sh
   make
   ```
3. Set PostgreSQL environment variables (production only):
   ```sh
   export PGHOST="db-acupinir.groupscholar.com"
   export PGPORT="23947"
   export PGUSER="ralph"
   export PGPASSWORD="<password>"
   export PGDATABASE="postgres"
   ```

## Usage
```sh
./bin/gs-policy-change init
./bin/gs-policy-change seed
./bin/gs-policy-change add --title "FAFSA rule update" --category "Compliance" --impact "High" --effective-date "2026-02-05" --owner "Policy Ops" --notes "Updated documentation required."
./bin/gs-policy-change show --id 3
./bin/gs-policy-change list --limit 10 --category "Compliance" --since "2026-01-01"
./bin/gs-policy-change report --by impact --since "2026-01-01" --until "2026-03-01"
./bin/gs-policy-change upcoming --days 45 --owner "Policy Ops"
```

## Testing
```sh
make test
```

## Notes
- Each project should use its own schema. This project uses `groupscholar_policy_change_tracker`.
- Do not commit credentials. Use environment variables only.
