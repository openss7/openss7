            if (idx = do_pass (msg, offset, vu)) {
                int     status = parm -> error__status;

                (void) do_pass (msg, type_SNMP_PDUs_rollback, vu);

                parm -> error__status = status;
            }
            else {
                (void) do_pass (msg, type_SNMP_PDUs_commit, vu);
                gc_set ();
            }
