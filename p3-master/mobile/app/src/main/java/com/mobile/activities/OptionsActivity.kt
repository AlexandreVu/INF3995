package com.mobile.activities

import android.content.Intent
import android.os.Bundle
import android.view.View
import androidx.appcompat.app.AlertDialog
import androidx.appcompat.app.AppCompatActivity
import androidx.constraintlayout.widget.ConstraintLayout
import com.mobile.HttpHandler
import com.mobile.R
import com.mobile.fragments.CourseSearchFragment
import com.mobile.fragments.StudentSearchFragment
import kotlinx.android.synthetic.main.option_view.*

class OptionsActivity : AppCompatActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.option_view)

        val isEdit = intent.getBooleanExtra("isEdit", false)
        if (!isEdit) {
            edit_information_btn.visibility = View.INVISIBLE
            val logoutButtonLayout = logout_btn.layoutParams as ConstraintLayout.LayoutParams
            logoutButtonLayout.topToBottom = consult_btn.id
        }

        val studentSearchFragment =
            StudentSearchFragment(this)
        val courseSearchFragment =
            CourseSearchFragment(this)
        val consultOptionsBuilder = AlertDialog.Builder(this@OptionsActivity)
        consultOptionsBuilder.setTitle(getString(R.string.option_popup_title))
        consultOptionsBuilder.setMessage(getString(R.string.option_popup_body))
        consultOptionsBuilder.setPositiveButton(getString(R.string.options_student_class)) { _, _ ->
            studentSearchFragment.show()
        }

        consultOptionsBuilder.setNeutralButton(getString(R.string.options_class_students)) { _, _ ->
            courseSearchFragment.show()
        }

        val consultOptionsDialog: AlertDialog = consultOptionsBuilder.create()
        edit_information_btn.setOnClickListener {
            val intent = Intent(applicationContext, EditActivity::class.java)
            startActivity(intent)
        }

        consult_btn.setOnClickListener {
            consultOptionsDialog.show()
        }

        edit_account_btn.setOnClickListener {
            val intent = Intent(applicationContext, UserActivity::class.java)
            startActivity(intent)
        }

        logout_btn.setOnClickListener {
            val httpHandler = HttpHandler(this)
            httpHandler.logout()
        }

    }

    @Override
    override fun onBackPressed() {
        super.onBackPressed()
        val frags = supportFragmentManager.fragments
        if (frags.size == 0) {
            val httpHandler = HttpHandler(this)
            httpHandler.logout()
        }
    }

}

