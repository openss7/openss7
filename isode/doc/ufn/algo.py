

PurportedName ::= SEQUENCE OF String
		-- simplication, as attribute types can optionally be 
		-- specified

		-- Each element of the Purported Name is a string
		-- which has been parsed from the BNF   

Attribute ::= SEQUENCE { 
	type OBJECT IDENTIFIER, 
	value ANY }

RDN ::= Attribute -- simplification, as can be multi-value

DN ::= SEQUENCE OF RDN

Environment ::= SEQUENCE OF DN


EnvironmentList ::= SEQUENCE OF SEQUENCE {
			lower-bound INTEGER,
			upper-bound INTEGER, 
			environment Environment }


friendlyMatch(p: PurportedName; el: EnvironmentList):  SET OF DN 
{
				-- Find correct environment
				
	IF length(el) == 0 THEN return(NULL);

	IF length(p) <= head(el).upper-bound
			&& length(p) >= head(el).lower-bound THEN
		return envMatch (p, head(el).environment);
	ELSE
		return(friendlyMatch(p, tail(el));
}


envMatch(p: PurportedName; e: Environment):  SET OF DN 
{
				-- Check elements of environment
				-- in the defined order
				
	matches: SET OF DN;
	
	IF length(e) == 0 THEN return(NULL);

	matches = purportedMatch(head(el).DN, p)
 	IF matches != NULL THEN 
		return(matches);
	ELSE
		return(envMatch(p, tail(e));
}


purportedMatch(base: DN; p: PurportedName): SET OF DN 
{
	s: String = head(p);
	matches: SET OF DN = NULL;

	IF length(p) == 1 THEN 
		IF length(base) == 0 THEN
			IF (matches = rootSearch(s)) != NULL THEN 
				return(matches);
			ELSE return(leafSearch(base, s, FALSE);
		ELSE IF length(base) == 1 THEN
			IF (matches = intSearch(base, s)) != NULL THEN
				return(matches);
			ELSE return(leafSearch(base, s, FALSE);
		ELSE 
			IF (matches = leafSearch(base, s, TRUE)) != NULL THEN
				return(matches);
			ELSE return(intsearch(base, s);


	IF length(base) == 0 THEN
		FOR x IN rootSearch(s) DO
			matches += (purportedMatch(x, tail(p));
	ELSE			
		FOR x IN intSearch(base, s) DO
			matches += (purportedMatch(x, tail(p));
	return(matches);
}			


-- General.  Might need to tighten the filter for short strings,
-- in order to stop being flooded.  Alternatively, this could be 
-- done if the loose search hists a size limit
	
rootSearch(s: String): SET OF DN
{
	IF length == 2 THEN
		return(search(NULL, FALSE, s, {CountryName, 
			FriendlyCountryName, OrganizationName},
			{exact}, {Country, Organisation}));
	ELSE
		return(search(NULL, FALSE, s, {OrganizationName,
			FriendlyCountryName}, {substring, approx},
			{Country, Organisation}));
}
		

intSearch( base: DN; s: String)
{
	IF present(base, OrgUnitName) THEN
		return(search(base, FALSE, s, {OrgUnitName},
			{substring, approx}, {OrgUnit}));
	ELSE IF present(base, OrganisationName) THEN
		return(search(base, FALSE, {OrgUnitName,
			LocalityName}, {substring, approx},
			{Organization, OrgUnit, Locality}));
	ELSE IF present(base, LocalityName) THEN
		return(search(base, FALSE, s, {OrganisationName},
			{substring, approx}, {Locality});
	ELSE
		return(search(base, false, s, {OrganisationName,
			LocalityName}, {substring, approx}, 
			{Organisation, Locality}));
}


present(d: DN; t: AttributeType): BOOLEAN
{
	FOR x IN d DO
		IF x.type == t THEN return(TRUE);
	return(FALSE);
}

leafSearch(base: DN; s: String; subtree: BOOLEAN) 
{	
	return(search(base, subtree, s, {CommonName, Surname,
		UserId}, {substring, approx}));
}


search(base: DN; subtrees: BOOLEAN; s: string;
	alist SET OF AttributeType; matchtypes SET OF MatchType 
	objectClasses SET OF ObjectClass OPTIONAL): SET OF DN 
{
	-- mapped onto Directory Search, with OR conjunction
	-- of filter items

	return dNSelect (s, search-results, alist);
}

read(base: DN; alist SET OF AttributeType): SET OF Attribute;
{
	-- mapped onto Directory Read
	-- Types repeated to deal with multiple values
	-- This would be implemented by returning selected info
	-- with the search operation
}

dNSelect(s: String; dlist SET OF DN; alist: SET OF AttributeType): SET OF DN
{
	exact, good: SET OF DN;

	FOR x IN dlist DO
		IF last(DN).Value == s THEN
			exact += x;
		ELSE IF FOR y IN read(x, alist) DO
			IF y.value == s THEN
				good += x;


	IF exact != NULL THEN return(exact);
	IF good != NULL THEN return(good);
	return(userQuery(dlist));
}	
				

userQuery(dlist SET OF DN): SET OF DN
{
	-- pass back up for manual checking
	-- user can strip all matches to force progres....
}


head()    -- return first element of list
tail()    -- return list with first element removed
length()  -- return size of list
last()	  -- return last element of list
