# GNA Codes

An algorithm to standardise addresses, and then converting them to Plus Codes (GNA Codes)

#What is here?

Here lies a bunch of codes that comprises the GNA Code parser. You can try them out at hipcoder.com.

But more of interest is the parser, which standardises addresses of various formats (not counting typos), e.g "6 Argyle Street, 6 ARGYLE ST., 6 argyle street" etc into one common form.

The file of most interest would be addieparser.ccp which contains the parsing code. Easyparser.cpp contains the helper classes.

#The Algorithm

Here's a very brief pseudocode of how the algorithm works.
```pseudocode
	while(easyparser.hasTextToRead()){}
		tryReadUnitNumber();
		else tryReadLotNumber();
		else tryReadFloorNumber();
		else tryReadHouseNumber();
		else tryReadStreetNameAndType();
		else tryLocality();
		else tryStateTerritory();
		else tryPostCode();

		if(something was parsed)
   		try parse commas etc;
		else
			incomplete parse, which means bad.

	}
```
So by doing things this way, you can parse address components out of order. The element parser for postcode, state territory etc is relatively simple as well, but you just have to keep in mind the different possible abbreviations. That's stored in `street_abbrevs.strext` which should have an exhaustive list. That was taken from an australia post PDF.

#Checking with GNAF

Next, after parsing the address, you want to query from the GNAF database. Don't set up the database using our hacky database system. Use a proper database, and then query by street name / suburb, and just go through all the results which matches the house number / unit number etc. In our case, we return the best results which match the street name, and use a heuristics to see which of the entries are the best closest match. It's just a naive test approach just for this proof of concept prototype.

Side note: We've been using a hacky database system (it's basically a mmap) to store the address files. It works blazingly fast in our system, but after uploading to a cheap $5 linode instance it turns out it's not as fast as we wished..

#What is next

An algorithm such as this can be used to standardise and cleanse addresses to a reasonable extent. However it won't do anything to ambiguous addresses such as "Corner of Urquhar St and Motwyn St", because it could point to any of the 4 corners.

There should be a unified australian portal for standardising addresses. People should be able to come in and look at what their "official plus code" is. And everyone will be happy then.

Thanks for reading!

###End
