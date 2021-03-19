package com.mobile

import android.content.Context
import android.content.Intent
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import androidx.appcompat.app.AlertDialog
import androidx.core.content.ContextCompat.startActivity
import androidx.recyclerview.widget.RecyclerView
import com.mobile.activities.PdfActivity
import kotlinx.android.synthetic.main.courses_list_item.view.*
import kotlinx.android.synthetic.main.student_list_item.view.*

class ListAdapter(
    private val items: MutableList<BlockChainInformation>,
    private val context: Context,
    private val isStudent: Boolean

) : RecyclerView.Adapter<ViewHolder>() {

    override fun onCreateViewHolder(parent: ViewGroup, viewType: Int): ViewHolder {
        if (isStudent) {
            return ViewHolder(
                LayoutInflater.from(context).inflate(
                    R.layout.student_list_item,
                    parent,
                    false
                ),
                context
            )
        } else {
            return ViewHolder(
                LayoutInflater.from(context).inflate(
                    R.layout.courses_list_item,
                    parent,
                    false
                ),
                context
            )
        }
    }

    override fun getItemCount(): Int {
        return items.size
    }

    override fun onBindViewHolder(holder: ViewHolder, position: Int) {
        holder.bind(items[position])
    }

}

class ViewHolder(view: View, private val context: Context) : RecyclerView.ViewHolder(view),
    View.OnClickListener {
    private val matricule = view.course_list_matricule_result
    private val grade = view.course_list_grade_result
    private val name = view.student_full_name
    private val courseCode = view.course_code
    private val courseName = view.course_name
    private val semesterCourse = view.semester
    private val courseGrade = view.grade

    init {
        val httpHandler: HttpHandler = HttpHandler(context)
        view.setOnClickListener(this)

        if (view.pdf_view_btn != null) {
            view.pdf_view_btn.setOnClickListener {
                val pdfString = httpHandler.getPdf(
                    courseCode = courseCode.text.toString(),
                    semester = Integer.parseInt(semesterCourse.text.toString())
                )
                if (!pdfString.isNullOrBlank()) {
                    val intent = Intent(context, PdfActivity::class.java)
                    intent.putExtra("pdf", pdfString)
                    startActivity(context, intent, null)
                } else {
                    val pdfLoadingErrorBuilder = AlertDialog.Builder(context)
                    pdfLoadingErrorBuilder.setTitle(context.getString(R.string.pdf_load_error_title))
                    pdfLoadingErrorBuilder.setMessage(context.getString(R.string.try_again))
                    val pdfLoadingErrorDialog: AlertDialog = pdfLoadingErrorBuilder.create()
                    pdfLoadingErrorDialog.show()
                }
            }
        }
    }

    override fun onClick(v: View?) {}

    fun bind(info: BlockChainInformation) {
        if (matricule != null) matricule.text = info.studentCode.toString()
        if (grade != null) grade.text = info.grade!!.toUpperCase()
        if (name != null) name.text =
            "${info.firstName!!.capitalize()}  ${info.lastName!!.capitalize()}"
        if (courseCode != null) courseCode.text = info.courseCode!!.toUpperCase()
        if (courseName != null) courseName.text = info.courseName!!.capitalize()
        if (semesterCourse != null) semesterCourse.text = info.trimester.toString()
        if (courseGrade != null) courseGrade.text = info.grade!!.toString().toUpperCase()
    }

}