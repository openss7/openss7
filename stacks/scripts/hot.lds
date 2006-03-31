SECTIONS
{
	.text :
	{
		*(.text.hot.get)
		*(.text.hot.read)
		*(.text.hot.in)
		*(.text.hot)
		*(.text.hot.out)
		*(.text.hot.write)
		*(.text.hot.put)
		*(.text)
		*(.text.unlikely)
	}
}
