
Pepy-refs [[ P struct pepy_refs * ]] ::= SEQUENCE {
    [1] IMPLICIT INTEGER [[ i parm->t_int ]],
    [2] IMPLICIT ENUMERATED [[ i parm->t_enum ]] {
        orange(1), apple(2), pear(3), bannana(4) },

    [3] IMPLICIT OCTET STRING [[ q parm->t_qbuf ]],
    [4] IMPLICIT OCTET STRING [[ s parm->t_string ]],
    [5] IMPLICIT OCTET STRING [[ o parm->t_ostring $ parm->t_olen ]],
    [6] IMPLICIT BIT STRING [[ t parm->t_pe ]],
    [7] IMPLICIT BIT STRING [[ x parm->t_bstring $ t_blen ]],
    [8] IMPLICIT BOOLEAN [[ b parm->t_bool ]],
    [9] IMPLICIT OBJECT IDENTIFIER [[ O parm->t_oid ]],
    [10] IMPLICIT REAL [[ r parm->t_real ]],
    [11] ANY [[ a parm->t_any ]],
    [12] Def-types [[ p parm->t_def ]],
    [13] Opt-types [[ p parm->t_opt ]] OPTIONAL

    }
