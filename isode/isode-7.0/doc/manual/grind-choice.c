
SearchResultData [[P struct ds_search_result *]]
        ::=
        CHOICE <D<(*parm)->srr_correlated>>
        <E<((parm->srr_correlated == FALSE) ? 2 : 1)>>
        {
        searchInfo
                SET [[ T struct ds_search_unit * $ srr_un.srr_unit ]]
......
                },
        uncorrelatedSearchInfo
                [0] SET OF [[ T struct ds_search_result * $ srr_un.srr_parts ]]
                    <<srr_next>> SearchResult [[p *]]
        }
