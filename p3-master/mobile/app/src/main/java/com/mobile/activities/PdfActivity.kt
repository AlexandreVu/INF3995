package com.mobile.activities

import android.os.Bundle
import android.util.Base64
import androidx.appcompat.app.AppCompatActivity
import com.mobile.R
import kotlinx.android.synthetic.main.pdf_view.*
import java.io.ByteArrayOutputStream
import java.io.InputStream

class PdfActivity : AppCompatActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.pdf_view)
        val pdf = intent.getStringExtra("pdf")!!
        pdf_view.fromBytes(convertBase64ToByteArray(pdf)).load()
    }

    fun getBytes(inputStream: InputStream): ByteArray? {
        val byteBuffer = ByteArrayOutputStream()
        val bufferSize = 10000
        val buffer = ByteArray(bufferSize)
        var len: Int
        while (inputStream.read(buffer).also { len = it } != -1) {
            byteBuffer.write(buffer, 0, len)
        }
        return byteBuffer.toByteArray()
    }

    fun convertByteArrayToBase64(byteArray: ByteArray): String {
        val flags = Base64.NO_PADDING.or(Base64.NO_WRAP)
        return Base64.encodeToString(byteArray, flags)
    }

    private fun convertBase64ToByteArray(base64: String): ByteArray {
        return Base64.decode(base64, Base64.NO_PADDING.or(Base64.NO_WRAP))
    }


}