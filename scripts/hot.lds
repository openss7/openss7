SECTIONS
{
	/DISCARD/		  : { *(.discard)
				      *(.discard.*) }

	.noaltinstructions	0 : { *(.altinstructions) }

	.text			0 : { *(.text.streams.hot.put)
				    *(.text.streams.hot.write)
				    *(.text.streams.hot.out)
				    *(.text.streams.hot)
				    *(.text.streams.hot.in)
				    *(.text.streams.hot.read)
				    *(.text.streams.hot.get)
				    *(.text)
				    *(.text.streams.unlikely) }
/*
	.text.unlikely		0 : { *(.text.streams.unlikely)		*(.text.unlikely)	}
 */
	__ksymtab		0 : { *(SORT(___ksymtab+*))		*(__ksymtab)		}
	__ksymtab_gpl		0 : { *(SORT(___ksymtab_gpl+*))		*(__ksymtab_gpl)	}
	__ksymtab_unused	0 : { *(SORT(___ksymtab_unused+*))	*(__ksymtab_unused)	}
	__ksymtab_unused_gpl	0 : { *(SORT(___ksymtab_unused_gpl+*))	*(__ksymtab_unused_gpl)	}
	__ksymtab_gpl_future	0 : { *(SORT(___ksymtab_gpl_future+*))	*(__ksymtab_gpl_future)	}
	__kcrctab		0 : { *(SORT(___kcrctab+*))		*(__kcrctab)		}
	__kcrctab_gpl		0 : { *(SORT(___kcrctab_gpl+*))		*(__kcrctab_gpl)	}
	__kcrctab_unused	0 : { *(SORT(___kcrctab_unused+*))	*(__kcrctab_unused)	}
	__kcrctab_unused_gpl	0 : { *(SORT(___kcrctab_unused_gpl+*))	*(__kcrctab_unused_gpl)	}
	__kcrctab_gpl_future	0 : { *(SORT(___kcrctab_gpl_future+*))	*(__kcrctab_gpl_future)	}

	.init_array		0 : ALIGN(8) { *(SORT(.init_array.*)) *(.init_array) }

	__jump_table		0 : ALIGN(8) { KEEP(*(__jump_table)) }
}
