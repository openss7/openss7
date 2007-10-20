-- qu.py

-- $Header: /xtel/isode/isode/dsap/x500as/RCS/qu.py,v 9.0 1992/06/16 12:14:33 isode Rel $
--
--
-- $Log: qu.py,v $
-- Revision 9.0  1992/06/16  12:14:33  isode
-- Release 8.0
--
--

--
--				  NOTICE
--
--    Acquisition, use, and distribution of this module and related
--    materials are subject to the restrictions of a license agreement.
--    Consult the Preface in the User's Manual for the full terms of
--    this agreement.
--
--


Quipu
	{
	ccitt
	data(9)
	pss(2342)
	ucl(19200300)
	quipu(99)
	directoryDefinitions(1)
	}

DEFINITIONS ::=

PREFIXES encode decode print

BEGIN

IMPORTS
	NameError ,
	ServiceError ,
	SecurityError
		FROM DAS
			{
			joint-iso-ccitt
			ds(5)
			modules(1)
			directoryAbstractService(2)
			}

	DistinguishedName ,
	RelativeDistinguishedName ,
	Attribute ,
	AttributeType
		FROM IF
			{
			joint-iso-ccitt
			ds(5)
			modules(1)
			informationFramework(1)
			}
	AlgorithmIdentifier
		FROM AF {
			joint-iso-ccitt
			ds(5)
			modules(1)
			authenticationFramework(7)
			};

-- ReliableROSData
-- 	::=
-- 	SEQUENCE
-- 	{
-- 	rosOperation
-- 		INTEGER ,
		-- the operation being applied
-- 	data
-- 		ANY ,
		-- the Operation Argument
-- 	oldVersion
-- 		ANY ,
		-- data version to which operation should be applied
-- 	newVersion
-- 		ANY
		-- version number which results from operation
-- 	}

AuthenticationPolicy
	::=
	ENUMERATED
	{
		trust-name (0),
		simple (1),
		strong (2)
	}

AuthenticationPolicySyntax [[P struct auth_policy *]]
	::=
	SEQUENCE
	{
	modification
		AuthenticationPolicy [[i ap_mod]],
	read-and-compare
		AuthenticationPolicy [[i ap_rnc]],
	list-and-search
		AuthenticationPolicy [[i ap_lns]]
	}

AccessSelector [[P struct acl_info *]]
        ::=
        CHOICE <<acl_selector_type>>
        {
        entry
                [0] NULL ,
        other
                [2] NULL ,
        prefix
                [3] NameList [[p acl_name]] ,
        group
                [4] NameList [[p acl_name]]
        }

AccessCategories [[P struct acl_info *]]
        ::=
        ENUMERATED [[i acl_categories]]
        {
                none (0) ,
                detect (1) ,
                compare (2) ,
                read (3) ,
                add (4) ,
                write (5)
        }

ACLInfo [[P struct acl_info *]]
        ::=
        SET OF [[ T struct acl_info * $ * ]] <<acl_next>>
                SEQUENCE [[T struct acl_info * $ *]]
                {
                        AccessSelector [[p * ]] ,
                        AccessCategories [[p *]]
                }

AttributeACL [[P struct acl_attr *]]
        ::=
        SEQUENCE
        {
                SET OF [[ T struct oid_seq  * $ aa_types ]] <<	oid_next >>
                        OBJECT IDENTIFIER [[O oid_oid]] ,
                ACLInfo [[p aa_acl]]
                    -- DEFAULT {{other , read}, {entry, write}}
                    OPTIONAL <E<test_acl_default(parm->aa_acl) != OK>><D<0>>
        }


ACLSyntax [[P struct acl *]]
        ::=
        SEQUENCE
        {
        childACL
                [0] ACLInfo [[p ac_child]]
                    -- DEFAULT {{other , read}, {entry, write}} ,
                    OPTIONAL <E<test_acl_default(parm->ac_child) != OK>><D<0>> ,
        entryACL
                [1] ACLInfo [[p ac_entry]]
                    -- DEFAULT {{other , read}, {entry, write}} ,
                    OPTIONAL <E<test_acl_default(parm->ac_entry) != OK>><D<0>> ,
        defaultAttributeACL
                [2] ACLInfo [[p ac_default]]
                    -- DEFAULT {{other , read}, {entry, write}} ,
                    OPTIONAL <E<test_acl_default(parm->ac_default) != OK>><D<0>> ,
                [3] SET OF [[ T struct acl_attr * $ ac_attributes ]] <<aa_next>>
                        AttributeACL [[p *]]
        }

SearchACLSyntax [[P struct sacl_info *]]
	::=
        %E{
		if ( parm->sac_scope != 0 ) {
			parm->sac_tmplen = 3;
			parm->sac_tmpbstr = (char *) int2strb_alloc(
			    parm->sac_scope, parm->sac_tmplen );
		}
        %}
	SEQUENCE
	{
	who
		AccessSelector [[p &parm->sac_info]],
	scope
		BITSTRING [[x sac_tmpbstr $ sac_tmplen]] {
			subtree (0),
			single-level (1),
			base-object (2)
		}
		%E{
			if ( parm->sac_tmpbstr )
				free( parm->sac_tmpbstr );
			parm->sac_tmpbstr = NULL;
		%}
		%D{
			(*parm)->sac_scope = strb2int( (*parm)->sac_tmpbstr, 
			    (*parm)->sac_tmplen );
			free( (*parm)->sac_tmpbstr );
			(*parm)->sac_tmpbstr = NULL;
		%},
		-- DEFAULT { subtree, single-level }
	access	CHOICE [[T struct sacl_info * $ *]] <<sac_access>> {
		    unsearchable-attributes
			[1] SET OF [[ T struct oid_seq * $ sac_types ]]
			    <<oid_next>> OBJECT IDENTIFIER [[O oid_oid]],
		    searchable   
			[2] SEQUENCE [[T struct sacl_info * $ *]] {
			    searchable-attributes
				SET OF [[T struct oid_seq * $ sac_types]]
				   <<oid_next>> OBJECT IDENTIFIER [[O oid_oid]],

			    max-results INTEGER [[i sac_maxresults]],

			    min-key-length-in-substring
				INTEGER [[i sac_minkeylength]]
				OPTIONAL <E<parm->sac_minkeylength != 0>>
					 <D<0>>,
				    -- give minimum length to prevent dumping
				    -- using a*, b*, etc.

			    zero-results-if-limit-exceeded
				BOOLEAN [[b sac_zeroifexceeded]] DEFAULT TRUE
				    -- only return results if search is precise
				    -- enough to match less than the limit.
				    -- this will normally be the case.
			}
		}
	}

ListACLSyntax [[P struct sacl_info *]]
	::=
	SearchACLSyntax (
		WITH COMPONENTS {
				who,
				scope ( single-level | base-object ),
				max-results
		}
	)
	%D{
		(*parm)->sac_tmpbstr = NULL;
	%}

NameList [[P struct dn_seq *]]
        ::=
        SET OF [[ T struct dn_seq * $ * ]] << dns_next>>
                DistinguishedName [[p dns_dn]]

EDBInfoSyntax [[P struct edb_info *]]
        ::=
        SEQUENCE
        {
        edb
                DistinguishedName [[p edb_name]] ,
        getFromDSA
                DistinguishedName [[p edb_getfrom]]
		    OPTIONAL,
        sendToDSAs
                NameList [[p edb_sendto]] ,
	getEDBAllowed
		NameList [[p edb_allowed]]
        }

RelativeEntry [[P struct entry *]]
        ::=
        SEQUENCE
        {
                RelativeDistinguishedName [[p parm->e_name]] ,
                SET OF [[ T attrcomp * $ e_attributes ]] << attr_link>>
                        Attribute [[p *]]
        }
	    %D{
		   /* Order them */
		   
		   Attr_Sequence as;
		   Attr_Sequence as_next;
		   Attr_Sequence newas = NULLATTR;
		   
		   for (as=(*parm)->e_attributes; as != NULLATTR; as = as_next) {
		       as_next = as->attr_link;
		       as->attr_link = NULLATTR;
		       newas = as_merge (newas,as);
		   }
		   (*parm)->e_attributes = newas;
	    %}

TreeStructureSyntax [[P struct tree_struct *]]
        ::=
-- Too messy fall back to pepy.
--      SET [[ T objectclass * $ tree_object ]]
--	{
--	mandatoryObjectClasses 
--		[1] SET OF OBJECT IDENTIFIER,
--	optionalObjectClasses
--		[2] SET OF OBJECT IDENTIFIER OPTIONAL,
--	permittedRDNs
--		[3] SET OF SET OF AttributeType
--	}
	ANY
	[[E treestruct_encode]] [[D treestruct_decode]] [[F tree_struct_free]]

EntryDataBlock [[P struct getedb_type *]]
        ::= ANY
-- Pepsy does not know about AVL trees...
--	SEQUENCE OF RelativeEntry

-- Pulled up
EDBVersion 
	::=
	UTCTime 

GetEntryDataBlockArgument [[P struct getedb_arg *]]
	::=
	SET
	{
	entry
		[0] DistinguishedName [[p ga_entry]] ,
	CHOICE [[ T struct getedb_arg * $ *]] << ga_type >> {
	       sendIfMoreRecentThan [1] EDBVersion [[s ga_version]],
	       getVersionNumber [2] NULL,
	       getEDB [3] NULL,
	       continuation [4] SEQUENCE [[T struct getedb_arg * $ *]] {
                        EDBVersion [[s ga_version]],
                        nextEntryPosition INTEGER [[i ga_nextEntryPos]] }
                },
	maxEntries [5] INTEGER [[i ga_maxEntries]] OPTIONAL 
		<E<parm->ga_maxEntries !=0>><D<0>>
	}

GetEntryDataBlockResult [[P struct getedb_result *]]
	::=
	SEQUENCE
	{
	versionHeld
		[0] EDBVersion [[s gr_version]] ,
		[1] EntryDataBlock [[p gr_type]]
			[[E EDB_encode]] [[D EDB_decode]] [[F EDB_free]]
			OPTIONAL <E<parm->gr_edb != 0>><D<0>>,
		nextEntryPostion INTEGER [[i gr_nextEntryPos]] OPTIONAL
		        <E<parm->gr_nextEntryPos !=0>><D<0>>
	}

ProtectedPassword [[P struct protected_password *]]
	::=
	SEQUENCE 
	{
		algorithm [0] AlgorithmIdentifier [[p alg_id]]
			OPTIONAL,
		salt [1] SET [[T struct protected_password *$ *]]
			{
			time1 [0] UTCTime [[s time1]]
				OPTIONAL,
			time2 [1] UTCTime [[s time2]]
				OPTIONAL,
			random1 [2] BIT STRING 
				OPTIONAL ,
			random2 [3] BIT STRING
				OPTIONAL 
			}
			OPTIONAL <<is_protected $ 1>>,
		password [2] OCTET STRING [[o passwd $ n_octets]]
	}


InheritedList [[P struct attrcomp * ]]
	::= 
	SET OF [[ T struct attrcomp * $ * ]]<<attr_link>>
		CHOICE [[ T struct attrcomp * $ * ]] <D<0>>
		       <E< parm->attr_value == NULLAV ? 1 : 2>>
			{
			AttributeType [[p attr_type]],
			Attribute [[p *]]
			}

InheritedAttribute [[P struct _InheritAttr *]]
	::= 
	SET
	{
	idefault [0]   InheritedList [[p i_default]]
			OPTIONAL,
	always	[1]	InheritedList [[p i_always]]
			OPTIONAL,
	objectclass	OBJECT IDENTIFIER [[O i_oid]]
			OPTIONAL
	}
	%D{

	/* Order the attribute lists */
	   
	Attr_Sequence as;
	Attr_Sequence as_next;
	Attr_Sequence newas = NULLATTR;
	   
	for (as=(*parm)->i_default; as != NULLATTR; as = as_next) {
	       as_next = as->attr_link;
	       as->attr_link = NULLATTR;
	       newas = as_merge (newas,as);
	}
	(*parm)->i_default = newas;

	newas = NULLATTR;

	for (as=(*parm)->i_always; as != NULLATTR; as = as_next) {
	       as_next = as->attr_link;
	       as->attr_link = NULLATTR;
	       newas = as_merge (newas,as);
	}
	(*parm)->i_always = newas;
	
	%}

DSAControl [[ P struct dsa_control * ]] 
	::= CHOICE << dsa_control_option >>
	{
               lockdsa [0] OptionalDN ,
               setLogLevel [1] PrintableString ,
               refresh [2] OptionalDN ,
               stopDSA [3] NULL ,
               unlock [4] OptionalDN ,
               resynch [5] OptionalDN ,
               changeTailor [6] PrintableString ,
               updateSlaveEDBs [7] PrintableString
        }

OptionalDN [[ P struct optional_dn * ]]
	::= CHOICE << offset >>
	{
		no-dn [0] NULL,
		selectedDN [1] DistinguishedName
	}

Call [[P quipu_call * ]] ::= SEQUENCE 
{
	protocol ENUMERATED {
		dap (0) ,
		dsp (1) ,
		quipudsp (2),
		internetdsp (3) } ,
	association-id INTEGER [[ i assoc_id ]] ,
	authorizationLevel AuthLevel [[ p authorizationLevel ]] ,
	initiated-by-dsa BOOLEAN [[ b initiated_by_dsa ]] ,
	usersDN DistinguishedName OPTIONAL,
	net-address OCTET STRING [[ s net_address ]] ,
	start-time UTCTime [[ s start_time ]] ,
	finish-time UTCTime [[ s finish_time ]] OPTIONAL ,
	pending-operations SET OF [[ T struct op_list * $ pending_ops ]] 
		Operation [[ p operation_list ]],
	invoked-operations SET OF [[ T struct op_list * $ invoked_ops ]] 
		Operation [[ p operation_list ]]
}

AuthLevel ::= ENUMERATED 
{
	none (0) ,
	indentified (1) ,
	simple (2) ,
	protected-simple (3) ,
	strong (4) 
}

Operation [[P struct ops * ]] ::= SEQUENCE
{
	invoke-id INTEGER [[ i invoke_id ]] ,
				-- unique handle on operation
	operation-id INTEGER [[ i operation_id ]] ,
				-- identify which operation read/search etc.
				-- values from X.519
	base-object DistinguishedName [[ p base_object ]] , 
	start-time UTCTime [[ s start_time ]],
	finish-time UTCTime [[ s finish_time ]] OPTIONAL ,
			 	-- cache info for monitoring
	chained-operations 
		SEQUENCE OF [[ T struct chain_list * $ chained_ops ]] 
			SubChainOps [[ p sub_chained_ops ]]
}

SubChainOps [[P struct sub_ch_list * ]] ::= SEQUENCE
{
	association-id INTEGER [[ i assoc_id ]],
	invoke-id INTEGER [[ i invok_id ]]
}

END
