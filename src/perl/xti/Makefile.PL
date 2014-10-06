use ExtUtils::MakeMaker;
WriteMakefile(
	'NAME' => 'Xti',
	'LIBS' => ['-lstreams', '-lxnet'],
	'OBJECT' => 'xti_wrap.o',
	'INC' => '-I../../include',
);
