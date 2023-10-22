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

    override fun onHook() {
    }

    override fun onXposedEvent() {
        YukiXposedEvent.events {

            onHandleLoadPackage {
                val context = XposedHelpers.callStaticMethod(
                    XposedHelpers.findClass("android.app.ActivityThread", null),
                    "currentApplication"
                ) as? Context
                context?.let { nonNullContext ->
                    clearCache(nonNullContext)
                }
            }
        }
    }

    private fun clearCache(context: Context) {
        val externalCacheDir = context.externalCacheDir
        if (externalCacheDir != null && externalCacheDir.exists()) {
            deleteFiles(externalCacheDir)
        }

        val cacheDir = context.cacheDir
        if (cacheDir != null && cacheDir.exists()) {
            deleteFiles(cacheDir)
        }

        val codeCacheDir = context.codeCacheDir
        if (codeCacheDir != null && codeCacheDir.exists()) {
            deleteFiles(codeCacheDir)
        }

        context.externalCacheDir?.mkdirs()
        context.cacheDir?.mkdirs()
        context.codeCacheDir?.mkdirs()
    }

    private fun deleteFiles(directory: File) {
        if (directory.isDirectory) {
            val files = directory.listFiles()
            if (files != null) {
                for (file in files) {
                    deleteFiles(file)
                }
            }
        }
        directory.delete()
    }
}