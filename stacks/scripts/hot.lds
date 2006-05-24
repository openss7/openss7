SECTIONS
{
	.text :
	{
		*(.text.hot.put)
		*(.text.hot.write)
		*(.text.hot.out)
		*(.text.hot)
		*(.text.hot.in)
		*(.text.hot.read)
		*(.text.hot.get)
		*(.text)
		*(.text.unlikely)
	}
}
