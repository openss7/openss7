   E-ANY ::= ANY [[ a * ]] [[ E get_pe ]]

   D-ANY ::= ANY [[ a * ]]  [[ D put_pe ]] [[ F fr_pe ]]

   DE-ANY ::= ANY [[ a * ]]  [[ D put_pe ]] [[ E get_pe ]]

    CodeOpt [[ P struct codedata * ]] ::= SEQUENCE {
        a [PRIVATE 5] BasicANY [[ p cd_a ]] OPTIONAL,
        b [35] E-ANY [[ p cd_b ]] OPTIONAL,
        c [45] EXPLICIT D-ANY [[ p cd_c ]] OPTIONAL <<cd_opt_set $ CD_C>>,
        d [1] DE-ANY [[ p cd_d ]] OPTIONAL <<cd_opt_set $ CD_D>>,
        e [15] IMPLICIT INTEGER [[ i cd_int ]] [[ E get_int ]] [[ D put_int ]]
                OPTIONAL << cd_opt_set $ CD_INT >>,
        f GraphicString [[ s cd_string ]] [[ E get_string ]][[ D put_string ]]
                OPTIONAL
    }

