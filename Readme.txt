﻿
How to build
ビルド方法


	[version,compiler]
このプロジェクトはVisualStudio2015でビルド、作成されています。
処理はunicode準拠で書かれていますがmetasequoiaとの連携部分がShiftJisとなっています。

x86,x64両バージョン対応です。


	[MetaswquoiaSDK]
公式よりdownloadしてください。

http://www.metaseq.net/jp/download/sdk/


	[project path]
MetasequoiaSDKのサンプルと同じ構成で作成されています。
同じ構成にするとライブラリのパスを再設定の手間が省けるでしょう。

mqsdk???\mqsdk\Station_ObjectComposition\


	[run & debug on VisualStudio]
DLLのためローカルリモートでデバッグを行います。
そのためホストプログラムであるMetasequoiaのパスをユーザの環境に合わせて設定する必要があります。
プロジェクト設定のデバッグでパスを通してください。

platform:x86(win32)
command：C:\Program Files %28x86%29\tetraface\Metasequoia4\Metaseq.exe
workpath：C:\Program Files %28x86%29\tetraface\Metasequoia4\

platform:x64
command:C:\Program Files\tetraface\Metasequoia4\Metaseq.exe
workpath:C:\Program Files\tetraface\Metasequoia4\

次にデバッグ機能を使用するにはpdbファイルをMetasequoiaの実行箇所にコピーする必要があります（リンク貼ると良いのかな？そもそもVS側が$outにあるpdbを読むだけで解決するのですが……）。
以下の4ファイルのパスを修正してください。

win32\debug\copy.bat
win32\release\copy.bat
x64\debug\copy.bat
x64\release\copy.bat

もしMetasequoia本体がprogramfiles\にインストールされている場合は、ファイルコピーのために管理者権限での実行が必要になりますので、コンパイル時にエラーが出るでしょう。
その場合は、管理者権限をつけてVisualStudioを実行してください。


	[license]
zlib License

ソフトウェアはas-is(現状のまま)ベースで利用される。著作者は利用により起こりうる損害に対する責任を負わない。
商用利用を含め、ソフトウエアの改変、配布は以下の制限の下で許可される:
このソフトウェアの原著作者であると詐称してはならない（ただしその表示義務は無い）。
ソースコードを改変した場合はオリジナルのままだと勘違いされないようにしなければならない。
このライセンス表示をソースコード配布物から削除してはならない。
ライセンスは、バイナリコードが配布されているならば、利用可能なソースコードを必要としない。
	
This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

   1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgment in the product documentation would be
   appreciated but is not required.

   2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.

   3. This notice may not be removed or altered from any source



