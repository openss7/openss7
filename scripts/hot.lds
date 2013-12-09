SECTIONS
{
	/DISCARD/		: { *(.discard) }

	.noaltinstructions	: { *(.altinstructions) }

	.text			: { *(.text.streams.hot.put)
				    *(.text.streams.hot.write)
				    *(.text.streams.hot.out)
				    *(.text.streams.hot)
				    *(.text.streams.hot.in)
				    *(.text.streams.hot.read)
				    *(.text.streams.hot.get)
				    *(.text)
				    *(.text.streams.unlikely) }
/*
	.text.unlikely		: { *(.text.streams.unlikely)		*(.text.unlikely)	}
 */
	__ksymtab		: { *(SORT(___ksymtab+*))		*(__ksymtab)		}
	__ksymtab_gpl		: { *(SORT(___ksymtab_gpl+*))		*(__ksymtab_gpl)	}
	__ksymtab_unused	: { *(SORT(___ksymtab_unused+*))	*(__ksymtab_unused)	}
	__ksymtab_unused_gpl	: { *(SORT(___ksymtab_unused_gpl+*))	*(__ksymtab_unused_gpl)	}
	__ksymtab_gpl_future	: { *(SORT(___ksymtab_gpl_future+*))	*(__ksymtab_gpl_future)	}
	__kcrctab		: { *(SORT(___kcrctab+*))		*(__kcrctab)		}
	__kcrctab_gpl		: { *(SORT(___kcrctab_gpl+*))		*(__kcrctab_gpl)	}
	__kcrctab_unused	: { *(SORT(___kcrctab_unused+*))	*(__kcrctab_unused)	}
	__kcrctab_unused_gpl	: { *(SORT(___kcrctab_unused_gpl+*))	*(__kcrctab_unused_gpl)	}
	__kcrctab_gpl_future	: { *(SORT(___kcrctab_gpl_future+*))	*(__kcrctab_gpl_future)	}
}
