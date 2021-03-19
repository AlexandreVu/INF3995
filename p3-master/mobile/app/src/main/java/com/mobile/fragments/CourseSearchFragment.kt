package com.mobile.fragments

import android.app.Dialog
import android.content.Context
import android.content.Intent
import android.os.Bundle
import android.view.Window
import androidx.appcompat.app.AlertDialog
import androidx.core.content.ContextCompat.startActivity
import com.mobile.BlockChainInformation
import com.mobile.HttpHandler
import com.mobile.ListManager
import com.mobile.R
import io.konform.validation.Valid
import io.konform.validation.Validation
import io.konform.validation.jsonschema.maxLength
import io.konform.validation.jsonschema.minLength
import io.konform.validation.jsonschema.pattern
import kotlinx.android.synthetic.main.course_search_fragment.*

class CourseSearchFragment(context: Context) : Dialog(context) {

    init {
        setCancelable(false)
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        requestWindowFeature(Window.FEATURE_NO_TITLE)
        setContentView(R.layout.course_search_fragment)
        setCanceledOnTouchOutside(true)
        val httpHandler =
            HttpHandler(this.context)

        search_course_btn.setOnClickListener {
            if (validateInformation()) {
                val studentList: ArrayList<BlockChainInformation> =
                    httpHandler.getClassStudents(
                        course_code_search_input.text.toString().trim(),
                        semester_course_search_input.text.toString().trim()
                    ) as ArrayList<BlockChainInformation>
                if (studentList.isNullOrEmpty()) {
                    val noResultsBuilder = AlertDialog.Builder(context)
                    noResultsBuilder.setTitle(context.getString(R.string.search_no_results_title))
                    noResultsBuilder.setMessage(context.getString(R.string.search_no_results_body))
                    val noResultsDialog: AlertDialog = noResultsBuilder.create()
                    noResultsDialog.show()
                } else {
                    val intent = Intent(context, ListManager()::class.java)
                    course_code_search_input.text.clear()
                    semester_course_search_input.text.clear()
                    intent.putExtra("isStudentList", true)
                    intent.putParcelableArrayListExtra("studentList", studentList)
                    startActivity(context, intent, null)
                }
            } else {
                val wrongFormatBuilder = AlertDialog.Builder(context)
                wrongFormatBuilder.setTitle(context.getString(R.string.course_search_format_error_title))
                wrongFormatBuilder.setMessage(context.getString(R.string.course_search_format_error_body))
                val wrongFormatDialog: AlertDialog = wrongFormatBuilder.create()
                wrongFormatDialog.show()
            }
        }
    }

    private fun validateInformation(): Boolean {
        val validateCourse = Validation<BlockChainInformation> {
            BlockChainInformation::trimester required {
                minLength(5)
                maxLength(5)
            }
            BlockChainInformation::courseCode required {
                minLength(7)
                maxLength(7)
                pattern("^[A-Za-z]{3}[0-9]{4}")
            }
        }

        val course = BlockChainInformation(
            courseCode = course_code_search_input.text.toString(),
            trimester = semester_course_search_input.text.toString(),
            studentCode = null,
            firstName = null,
            lastName = null,
            grade = null,
            courseName = null
        )
        return validateCourse(course) is Valid
    }
}