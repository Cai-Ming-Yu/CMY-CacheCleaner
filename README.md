# 彩銘羽 CacheCleaner
アプリの起動時にキャッシュをクリアするよ！

このリポジトリは[ここ](https://github.com/Xposed-Modules-Repo/Cai_Ming_Yu.CacheCleaner)と同じです。

Xposedを使用してアプリの起動時にキャッシュをクリアします。

This repo is exactly the same as [this one](https://github.com/Xposed-Modules-Repo/Cai_Ming_Yu.CacheCleaner).

Use Xposed to clear the software's cache data when it starts.

## Env
- LSPosed or any other framework that can use the Xposed module is installed
- LSPosedまたはXposed、その他フレームワークがインストールされている環境が必要です

## How to use
Install the latest version of the Xposed module released [here](https://github.com/Cai-Ming-Yu/Cai_Ming_Yu.CacheCleaner/releases), or package it into other software via frameworks such as LSPatch.

ここでリリースをしているXposedモジュールの最新版をインストールするか、LSPatchなどのフレームワークを使用して他のアプリに埋め込んでください。

If you are not packaging into other software using a tool like LSPatch, enable this module in an Xposed framework like LSPosed and select the required software.

LSPatchのようなツールを使って他のアプリに埋め込んでいない場合は、LSPosedのようなXposedフレームワークでこのモジュールを有効化し、必要なアプリを選択してください。

You can also build the software yourself by downloading the source code for this project(Software builds via e.g. ```./gradlew assembleRelease```).

このプロジェクトのソースコードをダウンロードして、自分自身でアプリをビルドする事もできます。(アプリのビルドは、``./gradlew assembleRelease`` などで行います)

Debug CI Version: https://github.com/Cai-Ming-Yu/CMY-CacheCleaner/actions

## [License](https://github.com/Cai-Ming-Yu/Cai_Ming_Yu.CacheCleaner/blob/C-M-Y/LICENSE)
