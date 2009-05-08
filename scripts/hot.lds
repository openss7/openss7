SECTIONS
{
	.text :
	{
		*(.text.streams.hot.put)
		*(.text.streams.hot.write)
		*(.text.streams.hot.out)
		*(.text.streams.hot)
		*(.text.streams.hot.in)
		*(.text.streams.hot.read)
		*(.text.streams.hot.get)
		*(.text)
		*(.text.streams.unlikely)
	}
}
