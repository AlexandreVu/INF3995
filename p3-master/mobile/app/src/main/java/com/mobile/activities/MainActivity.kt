package com.mobile.activities

import android.Manifest
import android.app.Activity
import android.content.pm.PackageManager
import android.os.Bundle
import android.os.StrictMode
import androidx.appcompat.app.AlertDialog
import androidx.appcompat.app.AppCompatActivity
import androidx.core.app.ActivityCompat
import com.mobile.HttpHandler
import com.mobile.R
import com.mobile.UserProfileData
import io.konform.validation.Valid
import io.konform.validation.Validation
import io.konform.validation.jsonschema.maxLength
import io.konform.validation.jsonschema.minLength
import kotlinx.android.synthetic.main.login_fragment.*

class MainActivity : AppCompatActivity() {
    private val requestExternalStorage = 1
    private val permissionsStorage = arrayOf(
        Manifest.permission.READ_EXTERNAL_STORAGE,
        Manifest.permission.WRITE_EXTERNAL_STORAGE
    )

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        val policy =
            StrictMode.ThreadPolicy.Builder().permitAll().build()
        StrictMode.setThreadPolicy(policy)
        verifyStoragePermissions(this)

        setContentView(R.layout.activity_main)
        layoutInflater.inflate(R.layout.login_fragment, null)


        val httpHandler = HttpHandler(this)
        login_btn.setOnClickListener {
            if (validateUser()) {
                httpHandler.login(
                    username = name_input.text.toString(),
                    password = password_input.text.toString()
                )
            } else {
                val connectionErrorBuilder = AlertDialog.Builder(this@MainActivity)
                connectionErrorBuilder.setTitle(getString(R.string.wrong_credentials_title))
                connectionErrorBuilder.setMessage(getString(R.string.wrong_credentials_body))
                val dialog: AlertDialog = connectionErrorBuilder.create()
                dialog.show()
            }
        }
    }

    private fun validateUser(): Boolean {
        val validateUser = Validation<UserProfileData> {
            UserProfileData::userName {
                minLength(3)
                maxLength(25)
            }
            UserProfileData::password {
                minLength(4)
                maxLength(25)
            }
        }

        val user = UserProfileData(
            name_input.text.toString(),
            password_input.text.toString()
        )

        return validateUser(user) is Valid
    }

    @Override
    override fun onBackPressed() {
        super.onBackPressed()
        finishAffinity()
    }

    private fun verifyStoragePermissions(activity: Activity) {
        val permission = ActivityCompat.checkSelfPermission(
            activity,
            Manifest.permission.WRITE_EXTERNAL_STORAGE
        )

        if (permission != PackageManager.PERMISSION_GRANTED) {
            ActivityCompat.requestPermissions(
                activity,
                permissionsStorage,
                requestExternalStorage
            )
        }
    }

}
