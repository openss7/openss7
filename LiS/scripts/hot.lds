SECTIONS
{
	.text :
	{
		*(.text.hot.put)
		*(.text.hot.read)
		*(.text.hot.in)
		*(.text.hot)
		*(.text.hot.out)
		*(.text.hot.write)
		*(.text.hot.get)
		*(.text)
		*(.text.unlikely)
	}
}
