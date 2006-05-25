SECTIONS
{
	.text :
	{
		*(.streams.hot.put)
		*(.streams.hot.write)
		*(.streams.hot.out)
		*(.streams.hot)
		*(.streams.hot.in)
		*(.streams.hot.read)
		*(.streams.hot.get)
		*(.text)
		*(.streams.unlikely)
	}
}
