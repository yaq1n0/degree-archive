BEGIN TRANSACTION;

CREATE TABLE CountryDate (
  dateRep TEXT,
  countriesAndTerritories TEXT,
  cases INT64,
  deaths INT64,
  PRIMARY KEY (dateRep, countriesAndTerritories)
);

CREATE TABLE Countries(
  countriesAndTerritories TEXT,
  geoId TEXT,
  countryterritoryCode TEXT,
  popData2019 INT64,
  continentExp TEXT,
  FOREIGN KEY (countriesAndTerritories) REFERENCES CountryDate(countriesAndTerritories)
);

CREATE TABLE Dates(
  dateRep TEXT,
  day INT8,
  month INT8,
  year INT16,
  FOREIGN KEY (dateRep) REFERENCES CountryDate(dateRep)
);

COMMIT;
