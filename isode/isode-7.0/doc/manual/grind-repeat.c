    Repeated [[ P struct repeats * ]]::= SEQUENCE {
        a SEQUENCE OF [[ T struct rep_int * $ rp_sq1 ]]
            <<r>> [1] INTEGER [[ i i ]],

        b SEQUENCE OF [[ T struct rep_elem * $ rp_sq2 ]] <<r_next>>
                Rep-elem [[ p * ]],

        c SET OF [[ T struct rep_int * $ rp_st1 ]] <<r>> INTEGER [[ i i ]],

        d SET OF [[ T struct rep_elem * $ rp_st2 ]] <<r_next>>
                [3] IMPLICIT Rep-elem [[ p * ]],

        e CHOICE [[ T struct repeats  * $ * ]] <<rp_choice>> {
                [1] INTEGER [[ i rp_int ]],
                [2] BOOLEAN [[ b rp_bool ]],
                [3] OCTET STRING [[ o rp_ostring $ rp_int ]]
            }
        }
