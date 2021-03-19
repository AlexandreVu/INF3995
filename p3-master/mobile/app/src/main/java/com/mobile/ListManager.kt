package com.mobile

import android.content.Intent
import android.os.Bundle
import android.view.View
import androidx.appcompat.app.AlertDialog
import androidx.appcompat.app.AppCompatActivity
import androidx.core.content.ContextCompat
import androidx.recyclerview.widget.LinearLayoutManager
import com.mobile.activities.PdfActivity
import kotlinx.android.synthetic.main.list_view.*

class ListManager : AppCompatActivity() {
    private var isStudentList = false

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.list_view)
        val httpHandler = HttpHandler(this)

        this.isStudentList = intent.getBooleanExtra("isStudentList", false)

        val list: ArrayList<BlockChainInformation>
        if (isStudentList) {
            list = intent.getParcelableArrayListExtra("studentList")!!
            search_title.text =
                "${list[0].courseCode!!.toUpperCase()}: ${list[0].courseName!!.capitalize()}"
            if (course_pdf_btn != null) {
                course_pdf_btn.setOnClickListener {
                    val pdfString = httpHandler.getPdf(
                        courseCode = list[0].courseCode.toString().toUpperCase(),
                        semester = Integer.parseInt(list[0].trimester.toString())
                    )
                    if (!pdfString.isNullOrBlank()) {
                        val intent = Intent(this, PdfActivity::class.java)
                        intent.putExtra("pdf", pdfString)
                        ContextCompat.startActivity(this, intent, null)
                    } else {
                        val pdfLoadingErrorBuilder = AlertDialog.Builder(this)
                        pdfLoadingErrorBuilder.setTitle(getString(R.string.pdf_load_error_title))
                        pdfLoadingErrorBuilder.setMessage(getString(R.string.try_again))
                        val pdfLoadingErrorDialog: AlertDialog = pdfLoadingErrorBuilder.create()
                        pdfLoadingErrorDialog.show()
                    }
                }
            }
            list_holder.layoutManager = LinearLayoutManager(this)
            list_holder.adapter =
                ListAdapter(list as MutableList<BlockChainInformation>, this, false)
        } else {
            course_pdf_btn.visibility = View.GONE
            list = intent.getParcelableArrayListExtra("courseList")!!
            search_title.text =
                "${list[0].firstName!!.capitalize()} ${list[0].lastName!!.capitalize()}"
            list_holder.layoutManager = LinearLayoutManager(this)
            list_holder.adapter =
                ListAdapter(list as MutableList<BlockChainInformation>, this, true)
        }

    }

}