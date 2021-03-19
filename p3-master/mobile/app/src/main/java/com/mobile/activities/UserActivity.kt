package com.mobile.activities

import android.os.Bundle
import android.widget.Button
import android.widget.EditText
import androidx.appcompat.app.AlertDialog
import androidx.appcompat.app.AppCompatActivity
import com.mobile.HttpHandler
import com.mobile.R
import kotlinx.android.synthetic.main.user_info_view.*


class UserActivity : AppCompatActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.user_info_view)
        val newPassword: EditText = findViewById(R.id.new_password)
        val passwordConfirm: EditText = findViewById(R.id.password_confirm)
        val saveChangesBtn: Button = findViewById(R.id.save_changes_btn)
        val httpHandler =
            HttpHandler(this)

        val unmatchingPassowrdsBuilder = AlertDialog.Builder(this@UserActivity)
        unmatchingPassowrdsBuilder.setTitle(getString(R.string.unmatching_passwords_title))
        unmatchingPassowrdsBuilder.setMessage(getString(R.string.unmatching_passwords_body))

        val unmatchingPasswordsDialog: AlertDialog = unmatchingPassowrdsBuilder.create()

        saveChangesBtn.setOnClickListener {
            if (newPassword.text.toString() != passwordConfirm.text.toString()) {
                passwordConfirm.background.setTint(resources.getColor(R.color.design_default_color_error))
                unmatchingPasswordsDialog.show()
            } else {
                httpHandler.changePassword(
                    newPassword = newPassword.text.toString(),
                    oldPassword = old_password.text.toString()
                )
            }
        }

    }

}
