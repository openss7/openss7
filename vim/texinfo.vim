syn match texinfoSpecialChar "@registeredsymbol{}"
syn match texinfoSpecialChar "@tabfig" contained
syn region texinfoBrcPrmAtCmd matchgroup=texinfoAtCmd start="@tabfig{" end="}" contains=texinfoSpecialChar,texinfoBrcPrmAtCmd
syn match texinfoSpecialChar "@tabref" contained
syn region texinfoBrcPrmAtCmd matchgroup=texinfoAtCmd start="@tabref{" end="}" contains=texinfoSpecialChar,texinfoBrcPrmAtCmd
syn match texinfoSpecialChar "@figure" contained
syn region texinfoBrcPrmAtCmd matchgroup=texinfoAtCmd start="@figure{" end="}" contains=texinfoSpecialChar,texinfoBrcPrmAtCmd
syn match texinfoSpecialChar "@figuresized" contained
syn region texinfoBrcPrmAtCmd matchgroup=texinfoAtCmd start="@figuresized{" end="}" contains=texinfoSpecialChar,texinfoBrcPrmAtCmd
syn match texinfoSpecialChar "@figref" contained
syn region texinfoBrcPrmAtCmd matchgroup=texinfoAtCmd start="@figref{" end="}" contains=texinfoSpecialChar,texinfoBrcPrmAtCmd
syn match texinfoSpecialChar "@manref" contained
syn region texinfoBrcPrmAtCmd matchgroup=texinfoAtCmd start="@manref{" end="}" contains=texinfoSpecialChar,texinfoBrcPrmAtCmd
syn match texinfoSpecialChar "@mantype" contained
syn region texinfoBrcPrmAtCmd matchgroup=texinfoAtCmd start="@mantype{" end="}" contains=texinfoSpecialChar,texinfoBrcPrmAtCmd
syn match texinfoSpecialChar "@ioctlref" contained
syn region texinfoBrcPrmAtCmd matchgroup=texinfoAtCmd start="@ioctlref{" end="}" contains=texinfoSpecialChar,texinfoBrcPrmAtCmd
syn match texinfoSpecialChar "@manpage" contained
syn region texinfoBrcPrmAtCmd matchgroup=texinfoAtCmd start="@manpage{" end="}" contains=texinfoSpecialChar,texinfoBrcPrmAtCmd
syn match texinfoSpecialChar "@type" contained
syn region texinfoBrcPrmAtCmd matchgroup=texinfoAtCmd start="@type{" end="}" contains=texinfoSpecialChar,texinfoBrcPrmAtCmd
syn match texinfoSpecialChar "@member" contained
syn region texinfoBrcPrmAtCmd matchgroup=texinfoAtCmd start="@member{" end="}" contains=texinfoSpecialChar,texinfoBrcPrmAtCmd
syn match texinfoSpecialChar "@constant" contained
syn region texinfoBrcPrmAtCmd matchgroup=texinfoAtCmd start="@constant{" end="}" contains=texinfoSpecialChar,texinfoBrcPrmAtCmd
syn match texinfoSpecialChar "@signal" contained
syn region texinfoBrcPrmAtCmd matchgroup=texinfoAtCmd start="@signal{" end="}" contains=texinfoSpecialChar,texinfoBrcPrmAtCmd
syn match texinfoSpecialChar "@errno" contained
syn region texinfoBrcPrmAtCmd matchgroup=texinfoAtCmd start="@errno{" end="}" contains=texinfoSpecialChar,texinfoBrcPrmAtCmd
syn match texinfoSpecialChar "@stropt" contained
syn region texinfoBrcPrmAtCmd matchgroup=texinfoAtCmd start="@stropt{" end="}" contains=texinfoSpecialChar,texinfoBrcPrmAtCmd
syn match texinfoSpecialChar "@streamio" contained
syn region texinfoBrcPrmAtCmd matchgroup=texinfoAtCmd start="@streamio{" end="}" contains=texinfoSpecialChar,texinfoBrcPrmAtCmd
syn match texinfoSpecialChar "@termio" contained
syn region texinfoBrcPrmAtCmd matchgroup=texinfoAtCmd start="@termio{" end="}" contains=texinfoSpecialChar,texinfoBrcPrmAtCmd
syn match texinfoSpecialChar "@sockio" contained
syn region texinfoBrcPrmAtCmd matchgroup=texinfoAtCmd start="@sockio{" end="}" contains=texinfoSpecialChar,texinfoBrcPrmAtCmd
syn match texinfoSpecialChar "@msg" contained
syn region texinfoBrcPrmAtCmd matchgroup=texinfoAtCmd start="@msg{" end="}" contains=texinfoSpecialChar,texinfoBrcPrmAtCmd
syn match texinfoSpecialChar "@header" contained
syn region texinfoBrcPrmAtCmd matchgroup=texinfoAtCmd start="@header{" end="}" contains=texinfoSpecialChar,texinfoBrcPrmAtCmd
syn match texinfoSpecialChar "@refman" contained
syn region texinfoBrcPrmAtCmd matchgroup=texinfoAtCmd start="@refman{" end="}" contains=texinfoSpecialChar,texinfoBrcPrmAtCmd
syn match texinfoSpecialChar "@xrefman" contained
syn region texinfoBrcPrmAtCmd matchgroup=texinfoAtCmd start="@xrefman{" end="}" contains=texinfoSpecialChar,texinfoBrcPrmAtCmd
syn match texinfoSpecialChar "@pxrefman" contained
syn region texinfoBrcPrmAtCmd matchgroup=texinfoAtCmd start="@pxrefman{" end="}" contains=texinfoSpecialChar,texinfoBrcPrmAtCmd
syn match texinfoSpecialChar "@prim" contained
syn region texinfoBrcPrmAtCmd matchgroup=texinfoAtCmd start="@prim{" end="}" contains=texinfoSpecialChar,texinfoBrcPrmAtCmd
syn match texinfoSpecialChar "@pval" contained
syn region texinfoBrcPrmAtCmd matchgroup=texinfoAtCmd start="@pval{" end="}" contains=texinfoSpecialChar,texinfoBrcPrmAtCmd
syn match texinfoSpecialChar "@psta" contained
syn region texinfoBrcPrmAtCmd matchgroup=texinfoAtCmd start="@psta{" end="}" contains=texinfoSpecialChar,texinfoBrcPrmAtCmd
syn match texinfoSpecialChar "@perr" contained
syn region texinfoBrcPrmAtCmd matchgroup=texinfoAtCmd start="@perr{" end="}" contains=texinfoSpecialChar,texinfoBrcPrmAtCmd
syn match texinfoSpecialChar "@parm" contained
syn region texinfoBrcPrmAtCmd matchgroup=texinfoAtCmd start="@parm{" end="}" contains=texinfoSpecialChar,texinfoBrcPrmAtCmd
syn match texinfoSpecialChar "@pioc" contained
syn region texinfoBrcPrmAtCmd matchgroup=texinfoAtCmd start="@pioc{" end="}" contains=texinfoSpecialChar,texinfoBrcPrmAtCmd
syn match texinfoSpecialChar "@mobj" contained
syn region texinfoBrcPrmAtCmd matchgroup=texinfoAtCmd start="@mobj{" end="}" contains=texinfoSpecialChar,texinfoBrcPrmAtCmd
syn match texinfoSpecialChar "@slanted" contained
syn region texinfoBrcPrmAtCmd matchgroup=texinfoAtCmd start="@slanted{" end="}" contains=texinfoSpecialChar,texinfoBrcPrmAtCmd

syn region texinfoMltlnAtCmd matchgroup=texinfoAtCmd start="^@verbatim\s*$"          end="^@end verbatim\s*$"         contains=ALL
