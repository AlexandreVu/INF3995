package com.mobile.activities

import android.app.Activity
import android.content.Intent
import android.database.Cursor
import android.os.Bundle
import android.provider.OpenableColumns
import androidx.appcompat.app.AlertDialog
import androidx.appcompat.app.AppCompatActivity
import com.mobile.BlockChainInformation
import com.mobile.R
import com.mobile.fragments.InformationEditionFragment
import io.konform.validation.Valid
import io.konform.validation.Validation
import io.konform.validation.jsonschema.pattern
import kotlinx.android.synthetic.main.edit_view.*
import java.io.InputStream


class EditActivity : AppCompatActivity() {
    private var base64Pdf: String = ""
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.edit_view)


        upload_file_btn.setOnClickListener {
            val intent = Intent()
            intent.type = "application/pdf"
            intent.action = Intent.ACTION_GET_CONTENT
            startActivityForResult(
                Intent.createChooser(
                    intent,
                    getString(R.string.pdf_select_text)
                ), 111
            )
        }

        add_student_btn.setOnClickListener {
            if (course_input.text.isBlank() || semester_code_input.text.isBlank() || course_name_input.text.isBlank()) {
                val missingInformationBuilder = AlertDialog.Builder(this@EditActivity)
                missingInformationBuilder.setTitle(getString(R.string.missing_information_title))
                missingInformationBuilder.setMessage(getString(R.string.missing_information_body))
                val missingInformationDialog: AlertDialog = missingInformationBuilder.create()
                missingInformationDialog.show()
            } else {
                if (validateInformation() && base64Pdf.isNotBlank()) {
                    val informationEditionFragment =
                        InformationEditionFragment(
                            this,
                            course_input.text.toString().trim(),
                            semester_code_input.text.toString().trim(),
                            course_name_input.text.toString().trim(),
                            base64Pdf
                        )
                    informationEditionFragment.show()
                } else if (base64Pdf.isBlank()) {
                    val missingPdfBuilder = AlertDialog.Builder(this@EditActivity)
                    missingPdfBuilder.setTitle(getString(R.string.missing_pdf_title))
                    missingPdfBuilder.setMessage(getString(R.string.missing_pdf_body))
                    val missingPdfDialog: AlertDialog = missingPdfBuilder.create()
                    missingPdfDialog.show()
                } else {
                    val wrongFormatBuilder = AlertDialog.Builder(this)
                    wrongFormatBuilder.setTitle(this.getString(R.string.course_search_format_error_title))
                    wrongFormatBuilder.setMessage(getString(R.string.transaction_format_err))
                    val wrongFormatDialog: AlertDialog = wrongFormatBuilder.create()
                    wrongFormatDialog.show()
                }
            }
        }
    }

    private fun validateInformation(): Boolean {
        val validateCourse = Validation<BlockChainInformation> {
            BlockChainInformation::trimester required {
                pattern("[0-9]{5}")
            }
            BlockChainInformation::courseName required {
                pattern("[A-zéèôà'\\-. ]{5,50}")
            }
            BlockChainInformation::courseCode required {
                pattern("^[A-Za-z]{3}[0-9]{4}")
            }

        }

        val course = BlockChainInformation(
            courseCode = course_input.text!!.toString(),
            trimester = semester_code_input.text!!.toString(),
            studentCode = null,
            firstName = null,
            lastName = null,
            grade = null,
            courseName = course_name_input.text.toString()
        )
        return validateCourse(course) is Valid
    }

    override fun onActivityResult(requestCode: Int, resultCode: Int, data: Intent?) {
        super.onActivityResult(requestCode, resultCode, data)
        val pdfActivity = PdfActivity()
        if (requestCode == 111 && resultCode == Activity.RESULT_OK && data != null) {
            val selectedPdfFromStorage = data.data
            val inputStream: InputStream? =
                contentResolver.openInputStream(selectedPdfFromStorage!!)
            base64Pdf = pdfActivity.convertByteArrayToBase64(pdfActivity.getBytes(inputStream!!)!!)
            val cursor: Cursor? = contentResolver.query(data.data!!, null, null, null, null)
            try {
                if (cursor != null && cursor.moveToFirst()) {
                    upload_file_btn.text =
                        cursor.getString(cursor.getColumnIndex(OpenableColumns.DISPLAY_NAME))
                }
            } finally {
                cursor!!.close()
            }
        } else {
            val pdfUploadErrorBuilder = AlertDialog.Builder(this@EditActivity)
            pdfUploadErrorBuilder.setTitle(getString(R.string.pdf_upload_error_title))
            pdfUploadErrorBuilder.setMessage(getString(R.string.pdf_upload_error_body))
            val pdfUploadErrorDialog: AlertDialog = pdfUploadErrorBuilder.create()
            pdfUploadErrorDialog.show()
        }
    }


}
