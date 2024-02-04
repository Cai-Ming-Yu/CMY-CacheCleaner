# 彩銘羽 CacheCleaner
ソフトウェアが起動したら、まずキャッシュをクリアする！

This repo is exactly the same as [this one](https://github.com/Xposed-Modules-Repo/Cai_Ming_Yu.CacheCleaner).

Use Xposed to clear the software's cache data when it starts.

## Env
- LSPosed or any other framework that can use the Xposed module is installed

## How to use
Install the latest version of the Xposed module released [here](https://github.com/Cai-Ming-Yu/Cai_Ming_Yu.CacheCleaner/releases), or package it into other software via frameworks such as LSPatch.

If you are not packaging into other software using a tool like LSPatch, enable this module in an Xposed framework like LSPosed and select the required software.

You can also build the software yourself by downloading the source code for this project(Software builds via e.g. ```./gradlew assembleRelease```).

Debug CI Version: https://github.com/Cai-Ming-Yu/CMY-CacheCleaner/actions

## [License](https://github.com/Cai-Ming-Yu/Cai_Ming_Yu.CacheCleaner/blob/C-M-Y/LICENSE)