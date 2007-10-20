    Single-type [[ P struct pepy_refs1 * ]] ::= SEQUENCE {
        a SEQUENCE [[ T struct pepy_refs1 * $ * ]] {
                [1] IMPLICIT INTEGER [[ i parm->t_int ]] OPTIONAL << opt_set $ O
PT_INT1 >>
            },
        b SEQUENCE [[ T struct pepy_refs1 * $ * ]] {
                [2] INTEGER [[ i parm->t_int1 ]] OPTIONAL << opt_set $ OPT_INT2
>>
            },

        c SET [[ T struct pepy_refs1 * $ *parm ]] {
            [3] IMPLICIT ENUMERATED [[ i parm->t_enum ]] {
orange(1), apple(2), pear(3), bannana(4) } OPTIONAL << opt_set $ OPT_ENUM1 >>
        },
        d SET [[ T struct pepy_refs1 *  $ *parm ]] {
            [4] ENUMERATED [[ i parm->t_enum1 ]] {
orange(1), apple(2), pear(3), bannana(4) } OPTIONAL << opt_set $ OPT_ENUM2 >>
        }
    }
