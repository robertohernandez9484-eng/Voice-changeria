package com.tudominio.voicetuner

import android.app.*
import android.content.Intent
import android.content.pm.PackageManager
import android.graphics.PixelFormat
import android.os.*
import android.view.*
import android.widget.*
import rikka.shizuku.Shizuku

class GameTurboOverlayService : Service() {
    private lateinit var windowManager: WindowManager
    private lateinit var overlayView: View

    external fun startVoiceEngine()
    external fun setVoiceEffect(effectId: Int)

    companion object { init { System.loadLibrary("voicetuner_native") } }

    override fun onCreate() {
        super.onCreate()
        startForegroundNotification()
        grantOverlayViaShizuku()
        setupFloatingPanel()
        startVoiceEngine()
    }

    private fun startForegroundNotification() {
        val channelId = "voice_tuner_channel"
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            val channel = NotificationChannel(channelId, "Sintonizador Activo", NotificationManager.IMPORTANCE_LOW)
            getSystemService(NotificationManager::class.java).createNotificationChannel(channel)
        }
        val notification = Notification.Builder(this, channelId)
            .setContentTitle("Sintonizador de Voz Activo")
            .setSmallIcon(android.R.drawable.ic_btn_speak_now)
            .build()
        startForeground(1, notification)
    }

    private fun grantOverlayViaShizuku() {
        if (Shizuku.pingBinder() && Shizuku.checkSelfPermission() == PackageManager.PERMISSION_GRANTED) {
            try {
                Shizuku.newProcess(arrayOf("appops", "set", packageName, "SYSTEM_ALERT_WINDOW", "allow"), null, null).waitFor()
            } catch (e: Exception) { e.printStackTrace() }
        }
    }

    private fun setupFloatingPanel() {
        windowManager = getSystemService(WINDOW_SERVICE) as WindowManager
        overlayView = LayoutInflater.from(this).inflate(R.layout.overlay_game_turbo, null)
        // Aquí deberías añadir listeners a los botones para llamar a setVoiceEffect(...)
        val params = WindowManager.LayoutParams(
            WindowManager.LayoutParams.WRAP_CONTENT,
            WindowManager.LayoutParams.WRAP_CONTENT,
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O)
                WindowManager.LayoutParams.TYPE_APPLICATION_OVERLAY
            else
                WindowManager.LayoutParams.TYPE_PHONE,
            WindowManager.LayoutParams.FLAG_NOT_FOCUSABLE or WindowManager.LayoutParams.FLAG_LAYOUT_IN_SCREEN,
            PixelFormat.TRANSLUCENT
        )
        params.gravity = Gravity.TOP or Gravity.END
        params.x = 20
        params.y = 200
        windowManager.addView(overlayView, params)
    }

    override fun onBind(intent: Intent?): IBinder? = null
}
