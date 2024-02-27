BEGIN TRANSACTION;

INSERT OR IGNORE INTO CountryDate(dateRep, countriesAndTerritories, cases, deaths)
SELECT
  dateRep, countriesAndTerritories, cases, deaths
FROM
  dataset;

INSERT INTO Countries(countriesAndTerritories, geoId, countryterritoryCode, popData2019, continentExp)
SELECT
  countriesAndTerritories, geoId, countryterritoryCode, popData2019, continentExp
FROM
  dataset;

INSERT INTO Dates(dateRep, day, month, year)
SELECT
  dateRep, day, month, year
FROM
  dataset;

COMMIT;
