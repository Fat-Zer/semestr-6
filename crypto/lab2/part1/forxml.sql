USE TaxiCompany
SELECT tblTaxists.TaxistName, tblClients.ClientName, tblClients.ClientPhone
FROM tblClients, tblQueries, tblTaxists
WHERE tblTaxists.TaxistCar='«»À'
	AND tblTaxists.TaxistID = tblQueries.TaxistID
	AND tblClients.ClientID = tblQueries.ClientID
ORDER BY tblTaxists.TaxistName
FOR XML AUTO--, ELEMENTS--, XMLDATA