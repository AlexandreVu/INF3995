package com.mobile

import android.os.Parcelable
import kotlinx.android.parcel.Parcelize

@Parcelize
data class BlockChainInformation(
    override val firstName: String?, override val lastName: String?,
    override val courseCode: String?,
    override val studentCode: Int?, override val grade: String?,
    override val trimester: String?, override val courseName: String?
) : Student, Course, Parcelable

interface Student {
    val firstName: String?
    val lastName: String?
    val studentCode: Int?
    val grade: String?
}

interface Course {
    val courseCode: String?
    val courseName: String?
    val trimester: String?
}

data class UserProfileData(
    val userName: String,
    val password: String
)