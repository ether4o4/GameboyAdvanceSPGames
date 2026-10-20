package com.whitetea.staticrush

import android.app.Activity
import android.os.Bundle
import android.view.View
import android.view.MotionEvent
import android.graphics.Canvas
import android.graphics.Color
import android.graphics.Paint
import kotlin.math.abs
import kotlin.math.min
import kotlin.math.max

class MainActivity : Activity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(GameView())
    }
    inner class GameView : View(this) {
        private val paint = Paint(3)
        private var playerX = 90f
        private var playerY = 0f
        private var vy = 0f
        private var vx = 0f
        private var downX = 0f
        private var downY = 0f
        private var lastTick = System.nanoTime()
        private var score = 0
        private var jumps = 0
        private var lastTopTap = 0L
        private val cells = mutableListOf(240f, 410f, 600f, 850f)
        override fun onDraw(c: Canvas) {
            val w = width.toFloat()
            val h = height.toFloat()
            val ground = h * .76f
            val dt = min(.05f, (System.nanoTime() - lastTick) / 1e9f)
            lastTick = System.nanoTime()
            vx *= (1f - .6f * dt)
            playerX = (playerX + vx * dt).coerceIn(25f, w - 25f)
            vy += 1100f * dt
            playerY += vy * dt
            if (playerY >= ground - 35f) {
                playerY = ground - 35f
                vy = 0f
                jumps = 0
            }
            c.drawColor(Color.rgb(18, 23, 34))
            paint.color = Color.rgb(50, 105, 115)
            c.drawRect(0f, ground, w, h, paint)
            paint.color = Color.rgb(130, 220, 235)
            for (i in 0..12) {
                val x = i * w / 12f
                c.drawLine(x, ground, x + 15f, h, paint)
            }
            for (i in cells.indices) {
                val x = cells[i]
                if (x > -20f && x < w + 20f) {
                    paint.color = Color.YELLOW
                    c.drawCircle(x, ground - 75f, 9f, paint)
                    if (abs(x - playerX) < 24f && abs(ground - 75f - playerY) < 28f) {
                        score++
                        cells[i] = w + 80f + i * 90f
                    }
                }
                cells[i] -= vx * dt * .3f
            }
            paint.color = Color.LTGRAY
            c.drawOval(playerX - 27f, playerY + 18f, playerX + 27f, playerY + 26f, paint)
            paint.color = Color.rgb(123, 55, 184)
            c.drawCircle(playerX, playerY, 20f, paint)
            paint.color = Color.CYAN
            paint.strokeWidth = 4f
            c.drawLine(playerX + 12f, playerY - 12f, playerX + 27f, playerY - 28f, paint)
            paint.textSize = 22f
            paint.color = Color.WHITE
            c.drawText("CELLS $score", w * .32f, 50f, paint)
            postInvalidateDelayed(16L)
        }
        override fun onTouchEvent(e: MotionEvent): Boolean {
            when (e.actionMasked) {
                MotionEvent.ACTION_DOWN -> {
                    downX = e.x
                    downY = e.y
                    return true
                }
                MotionEvent.ACTION_UP -> {
                    val dx = e.x - downX
                    val dy = e.y - downY
                    val tap = abs(dx) < 30f && abs(dy) < 30f
                    if (dy < -35f || (tap && e.y < height * .45f)) {
                        if (jumps < 2) {
                            vy = -470f
                            jumps++
                        }
                        lastTopTap = System.currentTimeMillis()
                    } else if (dx > 30f) {
                        vx = min(550f, max(250f, vx + 120f))
                    } else if (dx < -30f) {
                        vx = max(-550f, min(-250f, vx - 120f))
                    } else if (tap) {
                        vx = 0f
                    }
                    return true
                }
            }
            return true
        }
    }
}
