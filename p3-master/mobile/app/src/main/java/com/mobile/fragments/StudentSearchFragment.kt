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
import io.konform.validation.jsonschema.maximum
import io.konform.validation.jsonschema.minimum
import io.konform.validation.jsonschema.pattern
import kotlinx.android.synthetic.main.student_search_fragment.*

class StudentSearchFragment(context: Context) : Dialog(context) {

    init {
        setCancelable(false)
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        requestWindowFeature(Window.FEATURE_NO_TITLE)
        setContentView(R.layout.student_search_fragment)
        setCanceledOnTouchOutside(true)
        val httpHandler = HttpHandler(this.context)

        search_student_btn.setOnClickListener {
            if (validateInformation()) {
                val courseList: ArrayList<BlockChainInformation> =
                    httpHandler.getStudentClasses(
                        class_code_input.text.toString().trim().toUpperCase(),
                        semester_search_input.text.toString().trim(),
                        Integer.parseInt(matricule_input.text.toString().trim())
                    ) as ArrayList<BlockChainInformation>
                if (courseList.isNullOrEmpty()) {
                    val noResultsBuilder = AlertDialog.Builder(context)
                    noResultsBuilder.setTitle(context.getString(R.string.search_no_results_title))
                    noResultsBuilder.setMessage(context.getString(R.string.search_no_results_body))
                    val noResultsDialog: AlertDialog = noResultsBuilder.create()
                    noResultsDialog.show()
                } else {
                    val intent = Intent(
                        context, ListManager()::class.java
                    )
                    class_code_input.text.clear()
                    semester_search_input.text.clear()
                    matricule_input.text.clear()
                    intent.putExtra("isStudentList", false)
                    intent.putParcelableArrayListExtra("courseList", courseList)
                    startActivity(context, intent, null)
                }
            } else {
                val wrongFormatBuilder = AlertDialog.Builder(context)
                wrongFormatBuilder.setTitle(context.getString(R.string.course_search_format_error_title))
                wrongFormatBuilder.setMessage(context.getString(R.string.student_format_err))
                val wrongFormatDialog: AlertDialog = wrongFormatBuilder.create()
                wrongFormatDialog.show()
            }
        }
    }

    private fun validateInformation(): Boolean {
        val validateCourse = Validation<BlockChainInformation> {
            BlockChainInformation::trimester required {
                pattern("^[*]$|[0-9]{5}")
            }
            BlockChainInformation::courseCode required {
                pattern("^[*]$|^([A-Za-z]{3}[0-9]{4})")
            }
            BlockChainInformation::studentCode required {
                minimum(1000000)
                maximum(9999999)
            }
        }

        val course = BlockChainInformation(
            courseCode = class_code_input.text!!.toString(),
            trimester = semester_search_input.text!!.toString(),
            studentCode = Integer.parseInt(matricule_input.text.toString()),
            firstName = null,
            lastName = null,
            grade = null,
            courseName = null
        )

        return validateCourse(course) is Valid
    }

}