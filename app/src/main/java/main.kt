package CacheCleaner

import Cai_Ming_Yu.CacheCleaner.*
import java.io.File
import android.app.KeyguardManager
import android.content.Context
import android.content.pm.PackageManager
import de.robv.android.xposed.XC_MethodHook
import de.robv.android.xposed.XposedHelpers
import de.robv.android.xposed.callbacks.XC_LoadPackage
import com.highcapable.yukihookapi.hook.entity.YukiBaseHooker
import com.highcapable.yukihookapi.hook.xposed.bridge.event.YukiXposedEvent
import com.highcapable.yukihookapi.annotation.xposed.InjectYukiHookWithXposed
import com.highcapable.yukihookapi.hook.factory.configs
import com.highcapable.yukihookapi.hook.factory.encase
import com.highcapable.yukihookapi.hook.xposed.proxy.IYukiHookXposedInit
import com.highcapable.yukihookapi.hook.xposed.application.ModuleApplication

@InjectYukiHookWithXposed
object Hook : IYukiHookXposedInit {
    override fun onInit() = configs {
        debugLog {
            tag = "CacheCleaner"
            isEnable = false
            isRecord = false
            elements(TAG, PRIORITY, PACKAGE_NAME, USER_ID)
        }
        isDebug = BuildConfig.DEBUG
        isEnableModuleAppResourcesCache = true
        isEnableHookModuleStatus = true
        isEnableHookSharedPreferences = true
        isEnableDataChannel = true
    }

    override fun onHook() {}

    override fun onXposedEvent() {
        YukiXposedEvent.events {
            onHandleLoadPackage {
                val context = XposedHelpers.callStaticMethod(
                    XposedHelpers.findClass("android.app.ActivityThread", null), "currentApplication"
                ) as? Context
                context?.let { nonNullContext ->
                    if (it.packageName == "com.netease.cloudmusic") {
                        Music163(nonNullContext)
                    }
                    CacheCleaner(nonNullContext)
                }
            }
        }
    }

    private fun Music163(context: Context) {
        val Sdcard = XposedHelpers.callStaticMethod(
            XposedHelpers.findClass("android.os.Environment", null), "getExternalStorageDirectory"
        ) as File

        val SdcardCaches = listOf(
            "netease/cloudmusic/Cache/ImageCache",
            "netease/cloudmusic/Cache/MLog",
            "netease/cloudmusic/Cache/NewApk",
            "netease/cloudmusic/Cache/ShareResCache",
            "netease/cloudmusic/Cache/ShortVideo",
            "netease/cloudmusic/Cache/VideoCache",
            "netease/cloudmusic/Cache/UpgradeTemp"
        )

        for (SdcardCache in SdcardCaches) {
            val SdcardCacheDir = File(Sdcard, SdcardCache)
            if (SdcardCacheDir.exists() && SdcardCacheDir.isDirectory) {
                DelCaches(SdcardCacheDir)
            }
        }

        val AndroidDataFile = context.getExternalFilesDir(null)?.getAbsolutePath()

        val AndroidDataFileCaches = listOf(
            "Download", "nblog", "nCrash", "xcrash"
        )

        for (AndroidDataFileCache in AndroidDataFileCaches) {
            val AndroidDataFileCacheDir = File(AndroidDataFile, AndroidDataFileCache)
            if (AndroidDataFileCacheDir.exists() && AndroidDataFileCacheDir.isDirectory) {
                DelCaches(AndroidDataFileCacheDir)
            }
        }

        val Data = context.getDir("data", Context.MODE_PRIVATE)
        val DataFile = File(Data, "files")

        val DataFileCaches = listOf(
            "bundle/cache", "urs_sdk_log", "encryptStatistic/cache", "NetworkCache"
        )

        for (DataFileCache in DataFileCaches) {
            val DataFileCacheDir = File(DataFile, DataFileCache)
            if (DataFileCacheDir.exists() && DataFileCacheDir.isDirectory) {
                DelCaches(DataFileCacheDir)
            }
        }
    }

    private fun CacheCleaner(context: Context) {
        val AndroidCacheDir = context.externalCacheDir
        if (AndroidCacheDir != null && AndroidCacheDir.exists()) {
            DelCaches(AndroidCacheDir)
        }

        val CacheDir = context.cacheDir
        if (CacheDir != null && CacheDir.exists()) {
            DelCaches(CacheDir)
        }

        val CodeCacheDir = context.codeCacheDir
        if (CodeCacheDir != null && CodeCacheDir.exists()) {
            DelCaches(CodeCacheDir)
        }

        context.externalCacheDir?.mkdirs()
        context.cacheDir?.mkdirs()
        context.codeCacheDir?.mkdirs()
    }

    private fun DelCaches(CacheDir: File) {
        if (CacheDir.exists() && CacheDir.isDirectory) {
            CacheDir.deleteRecursively()
        }
    }
}