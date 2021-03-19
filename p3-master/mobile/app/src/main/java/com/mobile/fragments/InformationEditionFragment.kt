package com.mobile.fragments

import android.app.Dialog
import android.content.Context
import android.os.Bundle
import android.view.Window
import android.widget.ArrayAdapter
import androidx.appcompat.app.AlertDialog
import com.mobile.BlockChainInformation
import com.mobile.HttpHandler
import com.mobile.R
import com.mobile.Student
import io.konform.validation.Valid
import io.konform.validation.Validation
import io.konform.validation.jsonschema.maximum
import io.konform.validation.jsonschema.minimum
import io.konform.validation.jsonschema.pattern
import kotlinx.android.synthetic.main.information_add_fragment.*

class InformationEditionFragment(
    context: Context,
    private var courseCode: String,
    private var courseSemester: String,
    private var courseName: String,
    private var pdf: String
) :
    Dialog(context) {
    private val studentList: MutableList<Student> = mutableListOf()

    init {
        setCancelable(false)
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        requestWindowFeature(Window.FEATURE_NO_TITLE)
        setContentView(R.layout.information_add_fragment)
        setCanceledOnTouchOutside(true)
        val httpHandler = HttpHandler(context)

        val studentAddedBuilder = AlertDialog.Builder(context)
        studentAddedBuilder.setTitle(context.getString(R.string.information_added_title))
        studentAddedBuilder.setMessage(context.getString(R.string.add_other_students_body))
        studentAddedBuilder.setPositiveButton(context.getString(R.string.yes)) { _, _ ->
            first_name_input.text.clear()
            last_name_input.text.clear()
            student_code_input.text.clear()
            letter_grade_spinner.setSelection(0)
        }
        studentAddedBuilder.setNeutralButton(context.getString(R.string.send_students)) { _, _ ->
            if (validateInformation() && pdf.isNotBlank()) {
                httpHandler.sendTransaction(
                    courseCode = courseCode.toUpperCase(),
                    courseName = courseName,
                    semester = courseSemester,
                    students = studentList,
                    pdf = pdf
                )
            }

            this.dismiss()
        }

        val studentAddedDialog: AlertDialog = studentAddedBuilder.create()

        ArrayAdapter.createFromResource(
            context,
            R.array.Grades,
            android.R.layout.simple_spinner_item
        )
            .also { spinnerAdapter ->
                spinnerAdapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item)
                letter_grade_spinner.adapter = spinnerAdapter
            }

        save_student_btn.setOnClickListener {
            if (validateInformation()) {
                studentList.add(
                    BlockChainInformation(
                        firstName = first_name_input.text.toString(),
                        lastName = last_name_input.text.toString(),
                        studentCode = Integer.parseInt(student_code_input.text.toString()),
                        grade = letter_grade_spinner.selectedItem.toString(),
                        trimester = null,
                        courseCode = null,
                        courseName = null
                    )
                )
                studentAddedDialog.show()
            } else {
                val studentFormatErrorBuilder = AlertDialog.Builder(context)
                studentFormatErrorBuilder.setTitle(context.getString(R.string.course_search_format_error_title))
                studentFormatErrorBuilder.setMessage(context.getString(R.string.student_transaction_format_err))
                val studentFormatErrorDialog: AlertDialog = studentFormatErrorBuilder.create()
                studentFormatErrorDialog.show()
            }
        }
    }

    private fun validateInformation(): Boolean {
        val validateCourse = Validation<BlockChainInformation> {
            BlockChainInformation::firstName required {
                pattern("[A-zéèÉÈçëî-]+")
            }

            BlockChainInformation::lastName required {
                pattern("[A-zéèÉÈçëî-]+")
            }

            BlockChainInformation::studentCode required {
                minimum(1000000)
                maximum(9999999)
            }
        }

        val course = BlockChainInformation(
            courseCode = null,
            trimester = null,
            studentCode = Integer.parseInt(student_code_input.text.toString()),
            firstName = first_name_input.text.toString(),
            lastName = last_name_input.text.toString(),
            grade = letter_grade_spinner.selectedItem.toString(),
            courseName = null
        )
        return validateCourse(course) is Valid
    }
}