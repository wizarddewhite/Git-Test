set nocompatible              " be iMproved, required
filetype off                  " required

" Need to clone Vundle first:
"
" git clone https://github.com/VundleVim/Vundle.vim.git
" ~/.vim/bundle/Vundle.vim
"
" set the runtime path to include Vundle and initialize
set rtp+=~/.vim/bundle/Vundle.vim
call vundle#begin()
" alternatively, pass a path where Vundle should install plugins
"call vundle#begin('~/some/path/here')

" let Vundle manage Vundle, required
Plugin 'VundleVim/Vundle.vim'

Plugin 'vim-pandoc/vim-pandoc-syntax'
Plugin 'vim-pandoc/vim-pandoc'
" https://github.com/rust-lang/rust.vim
Plugin 'rust-lang/rust.vim'

" All of your Plugins must be added before the following line
call vundle#end()            " required
filetype plugin indent on    " required
" To ignore plugin indent changes, instead use:
"filetype plugin on
"
" Brief help
" :PluginList       - lists configured plugins
" :PluginInstall    - installs plugins; append `!` to update or just :PluginUpdate
" :PluginSearch foo - searches for foo; append `!` to refresh local cache
" :PluginClean      - confirms removal of unused plugins; append `!` to auto-approve removal
"
" see :h vundle for more details or wiki for FAQ
" Put your non-Plugin stuff after this line

let mapleader=","

"short cut for open ~/.vimrc
map <silent><leader>ee :e ~/.vimrc<cr>

"short cut for make
map <silent><leader>rm :make<cr>

"short cut for vsplit
map <silent><leader>vs :vsplit<cr>

"short cut for maximize the window
function Max_window()
	set columns=999
	set lines=999
endfunction
map <silent><leader>mw :call Max_window()<cr>

"color scheme
color desert

"""""""""""some basic settings"""""""""""""""
syntax on
"set mouse=a
set nu
set autoindent
set smartindent
set cindent
set backspace=2
set cmdheight=2
set shiftwidth=8
set ts=8
"set expandtab "expand tabs,by this new line will start with space
set encoding=prc
set nobackup
set wildmenu
set showcmd  "display partly entered command
set showmatch "show matching bracket for last ')'
set matchtime=5 "show matching time
set showmode    "show current mode
set ruler       "show the ruler
                 "error format for gcc, javac
set errorformat=\"%f\"\\\,\line\ %l.%c:%m\,\ %f:%l:%m
set incsearch      "incremental search
set hlsearch       "hilight search results
"set ignorecase
set textauto     "recognize ^M files
set nowrap
set dictionary=/usr/share/dict/words
set colorcolumn=80
set textwidth=78
set spell   "check spell

"打开的文件，显示在上次打开的位置
if has("autocmd")
   au BufReadPost * if line("'\"") > 0 && line("'\"") <= line("$")
    \| exe "normal! g'\"" | endif
endif

"set font
if has("gui_running")
	set guifont=DejaVu\ Sans\ Mono\ 14
elseif has("gui_macvim")
	set guifont=DejaVu_Sans_Mono:h14
elseif has("gui_win32")
	set guifont=DejaVu_Sans_Mono:h14
end

"代码折叠
"set fdm=indent or "set fdm=syntax
"set fdc=4
"set fdl=10 "默认情况下，打开所有的folder
"只对c 和 c++的文件 折叠，且输入引号时，自动补全
autocmd FileType c,cpp  setl fdm=syntax | setl fen | setl fdc=6 | setl fdl=10

"abbreviation
iabbrev #b /*************************************
iabbrev #e <Space>*************************************/
iabbrev /* /* */

"""""""""""Taglist Options"""""""""""""""
let Tlist_Show_One_File = 1
let Tlist_Exit_OnlyWindow = 1
let Tlist_File_Fold_Auto_Close = 1
let Tlist_WinWidth = 20
"let Tlist_Use_Right_Window = 1
map <silent> <F9> :TlistToggle<cr>


"""""""""""WinManager Option"""""""""""""""
let g:winManagerWindowLayout='FileExplorer|TagList'
nmap wm :WMToggle<cr>


"""""""""""MiniBuf Option"""""""""""""""
"<C-Tab> 向前走到一个文件
"<C-S-Tab> 向后走到一个文件
let g:miniBufExplMapCTabSwitchBufs = 1


"""""""""""Cscope Option"""""""""""""""
nmap <C-\>s :cs find s <C-R>=expand("<cword>")<CR><CR>
nmap <C-\>g :cs find g <C-R>=expand("<cword>")<CR><CR>
nmap <C-\>f :cs find f <C-R>=expand("<cword>")<CR><CR>
nmap <C-\>d :cs find d <C-R>=expand("<cword>")<CR><CR>
nmap <C-\>c :cs find c <C-R>=expand("<cword>")<CR><CR>
"读取cscope的数据库
if filereadable("cscope.out")
   execute "cs add cscope.out"
endif


"""""""""""a.vim Option"""""""""""""""
"按下<F12>，就可以在头文件和源文件之间切换
nnoremap <silent> <F12> :A<CR>


"""""""""""omnicpp Option"""""""""""""""
filetype plugin indent on
let OmniCpp_NamespaceSearch = 1
let OmniCpp_GlobalScopeSearch = 1
let OmniCpp_ShowAccess = 1
let OmniCpp_ShowPrototypeInAbbr = 1 " show function parameters
let OmniCpp_MayCompleteDot = 1 " autocomplete after .
let OmniCpp_MayCompleteArrow = 1 " autocomplete after ->
let OmniCpp_MayCompleteScope = 1 " autocomplete after ::
let OmniCpp_DefaultNamespaces = ["std", "_GLIBCXX_STD"]
" automatically open and close the popup menu / preview window
au CursorMovedI,InsertLeave * if pumvisible() == 0|silent! pclose|endif
set completeopt=menuone,menu,longest,preview


"""""""""""Auto Complete Option"""""""""""""""
autocmd FileType c,cpp  :inoremap " ""<ESC>i
"autocmd FileType c,cpp  :inoremap ' ''<ESC>i
" vim中 括号自动补全
:inoremap ( ()<ESC>i
:inoremap ) <c-r>=ClosePair(')')<CR>
:inoremap { {}<ESC>i
:inoremap } <c-r>=ClosePair('}')<CR>
:inoremap [ []<ESC>i
:inoremap ] <c-r>=ClosePair(']')<CR>
":inoremap < <><ESC>i
":inoremap > <c-r>=ClosePair('>')<CR>

function ClosePair(char)
if getline('.')[col('.') - 1] == a:char
    return "\<Right>"
else
    return a:char
endif
endf


"""""""""""showmarks Option"""""""""""""""
" Enable ShowMarks
let showmarks_enable = 1
" Show which marks
let showmarks_include = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
" Ignore help, quickfix, non-modifiable buffers
let showmarks_ignore_type = "hqm"
" Hilight lower & upper marks
let showmarks_hlline_lower = 1
let showmarks_hlline_upper = 1


"""""""""""c.vim Option"""""""""""""""
"chose different template for different file type
let g:C_Styles = { '*.c,*.h' : 'default', '*.cc,*.cpp,*.hh' : 'CPP' }


"""""""""""Quickfix Option"""""""""""""""
"设置 quickfix 的编码， 不过具体用什么还要再调整
function QfMakeConv()
   let qflist = getqflist()
   for i in qflist
      let i.text = iconv(i.text, "cp936", "utf-8")
   endfor
   call setqflist(qflist)
endfunction

au QuickfixCmdPost make call QfMakeConv()


"""""""""""lookupfile Option"""""""""""""""
let g:LookupFile_MinPatLength = 2 "最少输入2个字符才开始查找
let g:LookupFile_PreserveLastPattern = 0 "不保存上次查找的字符串
let g:LookupFile_PreservePatternHistory = 1 "保存查找历史
let g:LookupFile_AlwaysAcceptFirst = 1 "回车打开第一个匹配项目
let g:LookupFile_AllowNewFiles = 0 "不允许创建不存在的文件
if filereadable("./filenametags") "设置tag文件的名字,默认使用tags
    let g:LookupFile_TagExpr = '"./filenametags"'
endif
"映射LookupFile为,lk
nmap <silent> <leader>lk :LUTags<cr>
"映射LUBufs为,ll
nmap <silent> <leader>ll :LUBufs<cr>
"映射LUWalk为,lw
nmap <silent> <leader>lw :LUWalk<cr>
" lookup file with ignore case
function! LookupFile_IgnoreCaseFunc(pattern)
    let _tags = &tags
    try
        let &tags = eval(g:LookupFile_TagExpr)
        let newpattern = '\c' . a:pattern
        let tags = taglist(newpattern)
    catch
        echohl ErrorMsg | echo "Exception: " . v:exception | echohl NONE
        return ""
    finally
        let &tags = _tags
    endtry

    " Show the matches for what is typed so far.
    let files = map(tags, 'v:val["filename"]')
    return files
endfunction
let g:LookupFile_LookupFunc = 'LookupFile_IgnoreCaseFunc'


vmap sb "zdi<b><C-R>z</b><ESC>

" configuration for txtbrowser.vim
au BufEnter *.txt setlocal ft=txt
let TxtBrowser_Title_Level = 5

set mouse=a

imap <silent> <leader>sob Signed-off-by: Wei Yang <richard.weiyang@gmail.com><Esc>
imap <silent> <leader>rb Reviewed-by: Wei Yang <richard.weiyang@gmail.com><Esc>

call plug#begin('~/.vim/plugged')
" neoclide
" Reference for coc https://www.chrisatmachine.com/Neovim/04-vim-coc/
" After this change, use following command in vim to install rust-analyzer
" :PluginInstall
" :CocInstall coc-json coc-tsserver
" :CocInstall coc-vimlsp
" :CocInstall coc-rust-analyzer
Plug 'neoclide/coc.nvim', {'branch': 'release'}
Plug 'prabirshrestha/vim-lsp'
Plug 'mattn/vim-lsp-settings'
call plug#end()

" Install missing plugins on Vim startup
autocmd VimEnter *
  \  if len(filter(values(g:plugs), '!isdirectory(v:val.dir)'))
  \|   PlugInstall --sync | q
  \| endif

" GoTo code navigation. from https://github.com/neoclide/coc.nvim
nmap <silent> gd <Plug>(coc-definition)
nmap <silent> gy <Plug>(coc-type-definition)
nmap <silent> gi <Plug>(coc-implementation)
nmap <silent> gr <Plug>(coc-references)
