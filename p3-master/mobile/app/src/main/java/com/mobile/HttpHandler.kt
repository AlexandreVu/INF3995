package com.mobile

import android.content.Context
import android.content.Intent
import androidx.appcompat.app.AlertDialog
import androidx.core.content.ContextCompat.startActivity
import com.github.kittinunf.fuel.core.isClientError
import com.github.kittinunf.fuel.core.isServerError
import com.github.kittinunf.fuel.httpPost
import com.github.kittinunf.fuel.json.responseJson
import com.github.kittinunf.result.Result
import com.mobile.activities.MainActivity
import com.mobile.activities.OptionsActivity

class HttpHandler(private val context: Context) {
    companion object {
        private var address: String = "https://b9b24597.ngrok.io"
        var token: String = ""
        var isEditMode: Boolean = false
    }

    fun login(username: String, password: String): Boolean {
        val (_, response, result)
                = "$address/usager/login".httpPost().body(
            "{" +
                    "\"usager\":\"$username\",\n " +
                    "\"mot_de_passe\":\"$password\"" +
                    "}"
        ).responseJson()

        when (result) {
            is Result.Failure -> {
                val loginFailureBuilder = AlertDialog.Builder(context)
                loginFailureBuilder.setTitle(R.string.missing_information_title)

                when {
                    response.isClientError -> {
                        loginFailureBuilder.setMessage(context.getString(R.string.login_client_err))
                    }
                    response.isServerError -> {
                        loginFailureBuilder.setMessage(context.getString(R.string.login_server_err))
                    }
                    else -> {
                        loginFailureBuilder.setMessage(context.getString(R.string.generic_error))
                    }
                }
                val loginFailureDialog: AlertDialog = loginFailureBuilder.create()
                loginFailureDialog.show()
            }

            is Result.Success -> {
                val obj = result.value.obj()
                token = obj["token"].toString()
                isEditMode = obj["edition"].toString().toBoolean()
                val intent = Intent(context, OptionsActivity::class.java)
                intent.putExtra("isEdit", isEditMode)
                startActivity(context, intent, null)
            }
        }

        return isEditMode
    }

    fun logout() {
        val (_, _, result)
                = "$address/usager/logout".httpPost().body(
            "{" +
                    "\"jwt\":\"$token\"" +
                    "}"
        ).responseString()

        when (result) {
            is Result.Success -> {
                val intent = Intent(context, MainActivity::class.java)
                startActivity(context, intent, null)
            }
            is Result.Failure -> {
                val logoutErrorBuilder = AlertDialog.Builder(context)
                logoutErrorBuilder.setTitle(context.getString(R.string.logout_error_title))
                logoutErrorBuilder.setMessage(context.getString(R.string.generic_error))
                val logoutErrorDialog: AlertDialog = logoutErrorBuilder.create()
                logoutErrorDialog.show()
            }
        }
    }

    fun changePassword(oldPassword: String, newPassword: String) {
        val (_, _, result)
                = "$address/usager/motdepasse".httpPost().body(
            "{ " +
                    "\"jwt\":\"$token\"," +
                    "\"ancien\":\"$oldPassword\" , " +
                    "\"nouveau\":\"$newPassword\"" +
                    "}"
        ).responseString()
        val passwordChangeBuilder = AlertDialog.Builder(context)

        when (result) {
            is Result.Success -> {
                passwordChangeBuilder.setTitle(context.getString(R.string.password_change_success_title))
                passwordChangeBuilder.setMessage(context.getString(R.string.password_change_success_body))

                val intent = Intent(context, OptionsActivity::class.java)
                intent.putExtra("isEdit", isEditMode)
                startActivity(context, intent, null)
            }
            is Result.Failure -> {
                passwordChangeBuilder.setTitle("Erreur lors du changement de mot de passe.")
                passwordChangeBuilder.setMessage("Veuillez réessayer.")
            }
        }
        val passwordChangeDialog: AlertDialog = passwordChangeBuilder.create()
        passwordChangeDialog.show()

    }

    fun sendTransaction(
        courseCode: String,
        courseName: String,
        semester: String,
        students: MutableList<Student>,
        pdf: String
    ) {
        val (_, _, result)
                = "$address/transaction".httpPost().body(
            "{" +
                    "\"jwt\":\"$token\"," +
                    "\"sigle\":\"${courseCode.toUpperCase()}\"," +
                    "\"nom\":\"${courseName.capitalize()}\"," +
                    "\"trimestre\":${semester.toInt()}," +
                    "\"pdf\":\"$pdf\"," +
                    "\"resultats\":" +
                    "[" +
                    createStudentJson(students) +
                    "]" +
                    "}"
        ).responseString()

        val transactionBuilder = AlertDialog.Builder(context)
        when (result) {
            is Result.Success -> {
                transactionBuilder.setTitle(context.getString(R.string.transaction_success_title))
                transactionBuilder.setMessage(context.getString(R.string.transaction_success_body))
            }
            is Result.Failure -> {
                transactionBuilder.setTitle(R.string.transmission_error_title)
                transactionBuilder.setMessage(R.string.try_again)
            }
        }
        val transactionDialog: AlertDialog = transactionBuilder.create()
        transactionDialog.show()
    }

    fun getClassStudents(courseCode: String, semester: String): MutableList<BlockChainInformation> {
        val listOfStudents: MutableList<BlockChainInformation> = arrayListOf()
        val (_, _, result)
                = "$address/info/cours".httpPost()
            .body(
                "{" +
                        "\"jwt\":\"$token\"," +
                        "\"sigle\":\"${courseCode.toUpperCase()}\"," +
                        " \"trimestre\":${semester.toInt()}" +
                        "}"
            )
            .responseJson()
        when (result) {
            is Result.Success -> {
                if (!result.value.obj().isNull("resultats")) {
                    val jsonObject = result.value.obj()
                    val list = jsonObject.getJSONArray("resultats")
                    for (element in 0 until list.length()) {
                        val item = list.getJSONObject(element)
                        listOfStudents.add(
                            BlockChainInformation(
                                firstName = item["prenom"].toString(),
                                lastName = item["nom"].toString(),
                                studentCode = Integer.parseInt(item["matricule"].toString()),
                                grade = item["note"].toString(),
                                courseCode = courseCode,
                                trimester = semester,
                                courseName = jsonObject["nom"].toString()
                            )
                        )
                    }
                }
            }
            is Result.Failure -> {
                val getStudentsFailureBuilder = AlertDialog.Builder(context)
                getStudentsFailureBuilder.setTitle(context.getString(R.string.get_students_error_title))
                getStudentsFailureBuilder.setMessage(context.getString(R.string.try_again))
                val getStudentsFailureDialog: AlertDialog = getStudentsFailureBuilder.create()
                getStudentsFailureDialog.show()
            }
        }

        return listOfStudents
    }

    fun getStudentClasses(
        courseCode: String,
        semester: String,
        studentCode: Int
    ): MutableList<BlockChainInformation> {
        val listOfCourses: MutableList<BlockChainInformation> = arrayListOf()
        val (_, _, result)
                = "$address/info/etudiant".httpPost()
            .body(
                "{" +
                        "\"jwt\":\"$token\"," +
                        "\"sigle\":\"${courseCode.toUpperCase()}\", " +
                        "\"trimestre\":\"$semester\"," +
                        "\"matricule\":$studentCode" +
                        "}"
            )
            .responseJson()

        when (result) {
            is Result.Success -> {
                println("result success: ${result.value}")
                if (!result.value.obj().isNull("resultats")) {
                    val jsonObject = result.value.obj()
                    val list = jsonObject.getJSONArray("resultats")
                    for (element in 0 until list.length()) {
                        val item = list.getJSONObject(element)
                        listOfCourses.add(
                            BlockChainInformation(
                                trimester = item["trimestre"].toString(),
                                grade = item["note"].toString(),
                                courseCode = item["sigle"].toString(),
                                studentCode = studentCode,
                                firstName = jsonObject["prenom"].toString(),
                                lastName = jsonObject["nom"].toString(),
                                courseName = item["nom"].toString()
                            )
                        )
                    }
                }
            }
            is Result.Failure -> {
                println("result failure: ${result.error}")
                val getCoursesFailureBuilder = AlertDialog.Builder(context)
                getCoursesFailureBuilder.setTitle(context.getString(R.string.get_courses_error_title))
                getCoursesFailureBuilder.setMessage(context.getString(R.string.try_again))
                val getCoursesFailureDialog: AlertDialog = getCoursesFailureBuilder.create()
                getCoursesFailureDialog.show()
            }
        }

        return listOfCourses
    }

    fun getPdf(courseCode: String, semester: Int): String? {
        val (_, _, result)
                = "$address/fichier/notes".httpPost()
            .body(
                "{" +
                        "\"jwt\":\"$token\"," +
                        "\"sigle\":\"${courseCode.toUpperCase()}\", " +
                        "\"trimestre\":$semester" +
                        "}"
            )
            .responseJson()
        when (result) {
            is Result.Success -> {
                println("pdf success: ${result.value}")
                return result.value.obj()["pdf"].toString()
            }
            is Result.Failure -> {
                println("pdf failure: ${result.error}")
                val getPdfFailureBuilder = AlertDialog.Builder(context)
                getPdfFailureBuilder.setTitle(context.getString(R.string.pdf_error_title))
                getPdfFailureBuilder.setMessage(context.getString(R.string.try_again))
                val getPdfFailureDialog: AlertDialog = getPdfFailureBuilder.create()
                getPdfFailureDialog.show()
                return null
            }
        }
    }

    private fun createStudentJson(students: MutableList<Student>): String {
        val studentString: StringBuilder = StringBuilder()
        for (student in students) {
            studentString.append(
                "{ " +
                        "\"nom\":\"${student.lastName!!.capitalize()}\"," +
                        "\"prenom\":\"${student.firstName!!.capitalize()}\"," +
                        "\"matricule\":${student.studentCode!!.toInt()}," +
                        "\"note\":\"${student.grade!!.toUpperCase()}\"" +
                        "},"
            )
        }
        return studentString.substring(0, studentString.lastIndex - 1)
    }

}