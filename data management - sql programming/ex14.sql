BEGIN TRANSACTION;

SELECT SUM(cases), SUM(deaths)
FROM CountryDate;

COMMIT;
