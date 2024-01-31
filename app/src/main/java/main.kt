package tokyo.caimingyu.cachecleaner

import Cai_Ming_Yu.CacheCleaner.BuildConfig
import android.content.Context
import com.highcapable.yukihookapi.annotation.xposed.InjectYukiHookWithXposed
import com.highcapable.yukihookapi.hook.factory.configs
import com.highcapable.yukihookapi.hook.xposed.bridge.event.YukiXposedEvent
import com.highcapable.yukihookapi.hook.xposed.proxy.IYukiHookXposedInit
import de.robv.android.xposed.XposedHelpers
import java.io.File

@InjectYukiHookWithXposed
object main : IYukiHookXposedInit {
    override fun onInit() = configs {
        isDebug = BuildConfig.DEBUG
    }

    override fun onHook() {}

    override fun onXposedEvent() {
        YukiXposedEvent.events {
            onHandleLoadPackage {
                val context = XposedHelpers.callStaticMethod(
                    XposedHelpers.findClass("android.app.ActivityThread", null), "currentApplication"
                ) as? Context
                context?.let { nonNullContext ->
                    when (it.packageName) {
                        "com.netease.cloudmusic" -> Music163(nonNullContext)
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
            DelCaches(SdcardCacheDir)
        }

        val AndroidDataFile = context.getExternalFilesDir(null)?.absolutePath

        val AndroidDataFileCaches = listOf(
            "Download", "nblog", "nCrash", "xcrash"
        )

        for (AndroidDataFileCache in AndroidDataFileCaches) {
            val AndroidDataFileCacheDir = File(AndroidDataFile, AndroidDataFileCache)
            DelCaches(AndroidDataFileCacheDir)
        }

        val Data = context.getDir("data", Context.MODE_PRIVATE)
        val DataFile = File(Data, "files")

        val DataFileCaches = listOf(
            "bundle/cache", "urs_sdk_log", "encryptStatistic/cache", "NetworkCache"
        )

        for (DataFileCache in DataFileCaches) {
            val DataFileCacheDir = File(DataFile, DataFileCache)
            DelCaches(DataFileCacheDir)
        }
    }

    private fun CacheCleaner(context: Context) {
        val AndroidCacheDir = context.externalCacheDir
        DelCaches(AndroidCacheDir)

        val CacheDir = context.cacheDir
        DelCaches(CacheDir)

        val CodeCacheDir = context.codeCacheDir
        DelCaches(CodeCacheDir)

        context.externalCacheDir?.mkdirs()
        context.cacheDir?.mkdirs()
        context.codeCacheDir?.mkdirs()
    }

    private fun DelCaches(CacheDir: File?) {
        if (CacheDir != null && CacheDir.exists() && CacheDir.isDirectory) {
            CacheDir.deleteRecursively()
        }
    }
}