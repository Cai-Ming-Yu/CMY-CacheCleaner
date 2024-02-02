package tokyo.caimingyu.cachecleaner

import Cai_Ming_Yu.CacheCleaner.BuildConfig
import android.content.Context
import com.highcapable.yukihookapi.annotation.xposed.InjectYukiHookWithXposed
import com.highcapable.yukihookapi.hook.factory.configs
import com.highcapable.yukihookapi.hook.xposed.bridge.event.YukiXposedEvent
import com.highcapable.yukihookapi.hook.xposed.proxy.IYukiHookXposedInit
import de.robv.android.xposed.XposedHelpers
import de.robv.android.xposed.callbacks.XC_LoadPackage
import java.io.File

class Server {

    companion object {
        fun doHook(it: XC_LoadPackage.LoadPackageParam) {
            val context = XposedHelpers.callStaticMethod(
                XposedHelpers.findClass("android.app.ActivityThread", null), "currentApplication"
            ) as? Context
            context?.let { nonNullContext ->
                cacheCleaner(nonNullContext)
                when (it.packageName) {
                    "com.netease.cloudmusic" -> music163(nonNullContext)
                    else -> {}
                }
            }
        }

        private fun music163(context: Context) {
            val sdcard = XposedHelpers.callStaticMethod(
                XposedHelpers.findClass("android.os.Environment", null), "getExternalStorageDirectory"
            ) as File

            val sdcardCaches = listOf(
                "netease/cloudmusic/Cache/ImageCache",
                "netease/cloudmusic/Cache/MLog",
                "netease/cloudmusic/Cache/NewApk",
                "netease/cloudmusic/Cache/ShareResCache",
                "netease/cloudmusic/Cache/ShortVideo",
                "netease/cloudmusic/Cache/VideoCache",
                "netease/cloudmusic/Cache/UpgradeTemp"
            )

            for (sdcardCache in sdcardCaches) {
                val sdcardCacheDir = File(sdcard, sdcardCache)
                delCaches(sdcardCacheDir)
            }

            val androidDataFile = context.getExternalFilesDir(null)?.absolutePath

            val androidDataFileCaches = listOf(
                "Download", "nblog", "nCrash", "xcrash"
            )

            for (androidDataFileCache in androidDataFileCaches) {
                val androidDataFileCacheDir = File(androidDataFile, androidDataFileCache)
                delCaches(androidDataFileCacheDir)
            }

            val data = context.getDir("data", Context.MODE_PRIVATE)
            val dataFile = File(data, "files")

            val dataFileCaches = listOf(
                "bundle/cache", "urs_sdk_log", "encryptStatistic/cache", "NetworkCache"
            )

            for (dataFileCache in dataFileCaches) {
                val dataFileCacheDir = File(dataFile, dataFileCache)
                delCaches(dataFileCacheDir)
            }
        }

        private fun cacheCleaner(context: Context) {
            delCaches(context.externalCacheDir)
            delCaches(context.cacheDir)
            delCaches(context.codeCacheDir)
        }

        private fun delCaches(cacheDir: File?) {
            if (cacheDir != null && cacheDir.exists() && cacheDir.isDirectory) {
                cacheDir.deleteRecursively()
                cacheDir.mkdirs()
            }
        }
    }
}

@InjectYukiHookWithXposed
object Main : IYukiHookXposedInit {
    override fun onInit() = configs { isDebug = BuildConfig.DEBUG }

    override fun onHook() {}

    override fun onXposedEvent() {
        YukiXposedEvent.events { onHandleLoadPackage { Server.doHook(it) } }
    }
}