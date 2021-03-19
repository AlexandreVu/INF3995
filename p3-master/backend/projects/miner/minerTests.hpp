void testBlockChain(){
    Blockchain blockchain("blockchain.json");
    blockchain.initializeBlockchain();

    Block test1 = Block(blockchain.getLastIndex(), "test1");
    blockchain.addBlock(test1);

    Block test2 = Block(blockchain.getLastIndex(), "test2");
    blockchain.addBlock(test2);

    Block test3 = Block(blockchain.getLastIndex(), "test3");
    blockchain.addBlock(test3);

    Block test4 = Block(blockchain.getLastIndex(), "validation test");
    test4.mineBlock(5);
    Block test5 = Block(0, "block invalid");

    cout << blockchain.validateBlock(test4) << endl;
    cout << blockchain.validateBlock(test5) << endl;
    blockchain.saveBlockchain();
}

void testDatabase()
{
    int studentCode1 = 1839678;
    int studentCode2 = 1903967;

    char *firstNameStudent1 = "Bob";
    char *firstNameStudent2 = "Levis";

    char *lastNameStudent1 = "Tremblay";
    char *lastNameStudent2 = "Theriault";

    char *courseNumber1 = "INF3990";
    char *courseNumber2 = "INF4420a";
    char *courseNumber3 = "PHS1101";

    int trimester1 = 20171;
    int trimester2 = 20192;
    
    char *courseName1 = "Projet 3";
    char *courseName2 = "Securite Informatique";
    char *courseName3 = "Mecanique pour ingenieur";

    char *grade1 = "A";
    char *grade2 = "B";
    char *updateGrade = "C";

    int blockId1 = 1;
    int blockId2 = 2;
    int blockId3 = 3;
    int blockId4 = 4;
    int blockId5 = 5;
    int updateBlockId = 35;
    
    char* pdf = "JVBERi0xLjMNCiXi48/TDQoNCjEgMCBvYmoNCjw8DQovVHlwZSAvQ2F0YWxvZw0KL091dGxpbmVzIDIgMCBSDQovUGFnZXMgMyAwIFINCj4+DQplbmRvYmoNCg0KMiAwIG9iag0KPDwNCi9UeXBlIC9PdXRsaW5lcw0KL0NvdW50IDANCj4+DQplbmRvYmoNCg0KMyAwIG9iag0KPDwNCi9UeXBlIC9QYWdlcw0KL0NvdW50IDINCi9LaWRzIFsgNCAwIFIgNiAwIFIgXSANCj4+DQplbmRvYmoNCg0KNCAwIG9iag0KPDwNCi9UeXBlIC9QYWdlDQovUGFyZW50IDMgMCBSDQovUmVzb3VyY2VzIDw8DQovRm9udCA8PA0KL0YxIDkgMCBSIA0KPj4NCi9Qcm9jU2V0IDggMCBSDQo+Pg0KL01lZGlhQm94IFswIDAgNjEyLjAwMDAgNzkyLjAwMDBdDQovQ29udGVudHMgNSAwIFINCj4+DQplbmRvYmoNCg0KNSAwIG9iag0KPDwgL0xlbmd0aCAxMDc0ID4+DQpzdHJlYW0NCjIgSg0KQlQNCjAgMCAwIHJnDQovRjEgMDAyNyBUZg0KNTcuMzc1MCA3MjIuMjgwMCBUZA0KKCBBIFNpbXBsZSBQREYgRmlsZSApIFRqDQpFVA0KQlQNCi9GMSAwMDEwIFRmDQo2OS4yNTAwIDY4OC42MDgwIFRkDQooIFRoaXMgaXMgYSBzbWFsbCBkZW1vbnN0cmF0aW9uIC5wZGYgZmlsZSAtICkgVGoNCkVUDQpCVA0KL0YxIDAwMTAgVGYNCjY5LjI1MDAgNjY0LjcwNDAgVGQNCigganVzdCBmb3IgdXNlIGluIHRoZSBWaXJ0dWFsIE1lY2hhbmljcyB0dXRvcmlhbHMuIE1vcmUgdGV4dC4gQW5kIG1vcmUgKSBUag0KRVQNCkJUDQovRjEgMDAxMCBUZg0KNjkuMjUwMCA2NTIuNzUyMCBUZA0KKCB0ZXh0LiBBbmQgbW9yZSB0ZXh0LiBBbmQgbW9yZSB0ZXh0LiBBbmQgbW9yZSB0ZXh0LiApIFRqDQpFVA0KQlQNCi9GMSAwMDEwIFRmDQo2OS4yNTAwIDYyOC44NDgwIFRkDQooIEFuZCBtb3JlIHRleHQuIEFuZCBtb3JlIHRleHQuIEFuZCBtb3JlIHRleHQuIEFuZCBtb3JlIHRleHQuIEFuZCBtb3JlICkgVGoNCkVUDQpCVA0KL0YxIDAwMTAgVGYNCjY5LjI1MDAgNjE2Ljg5NjAgVGQNCiggdGV4dC4gQW5kIG1vcmUgdGV4dC4gQm9yaW5nLCB6enp6ei4gQW5kIG1vcmUgdGV4dC4gQW5kIG1vcmUgdGV4dC4gQW5kICkgVGoNCkVUDQpCVA0KL0YxIDAwMTAgVGYNCjY5LjI1MDAgNjA0Ljk0NDAgVGQNCiggbW9yZSB0ZXh0LiBBbmQgbW9yZSB0ZXh0LiBBbmQgbW9yZSB0ZXh0LiBBbmQgbW9yZSB0ZXh0LiBBbmQgbW9yZSB0ZXh0LiApIFRqDQpFVA0KQlQNCi9GMSAwMDEwIFRmDQo2OS4yNTAwIDU5Mi45OTIwIFRkDQooIEFuZCBtb3JlIHRleHQuIEFuZCBtb3JlIHRleHQuICkgVGoNCkVUDQpCVA0KL0YxIDAwMTAgVGYNCjY5LjI1MDAgNTY5LjA4ODAgVGQNCiggQW5kIG1vcmUgdGV4dC4gQW5kIG1vcmUgdGV4dC4gQW5kIG1vcmUgdGV4dC4gQW5kIG1vcmUgdGV4dC4gQW5kIG1vcmUgKSBUag0KRVQNCkJUDQovRjEgMDAxMCBUZg0KNjkuMjUwMCA1NTcuMTM2MCBUZA0KKCB0ZXh0LiBBbmQgbW9yZSB0ZXh0LiBBbmQgbW9yZSB0ZXh0LiBFdmVuIG1vcmUuIENvbnRpbnVlZCBvbiBwYWdlIDIgLi4uKSBUag0KRVQNCmVuZHN0cmVhbQ0KZW5kb2JqDQoNCjYgMCBvYmoNCjw8DQovVHlwZSAvUGFnZQ0KL1BhcmVudCAzIDAgUg0KL1Jlc291cmNlcyA8PA0KL0ZvbnQgPDwNCi9GMSA5IDAgUiANCj4+DQovUHJvY1NldCA4IDAgUg0KPj4NCi9NZWRpYUJveCBbMCAwIDYxMi4wMDAwIDc5Mi4wMDAwXQ0KL0NvbnRlbnRzIDcgMCBSDQo+Pg0KZW5kb2JqDQoNCjcgMCBvYmoNCjw8IC9MZW5ndGggNjc2ID4+DQpzdHJlYW0NCjIgSg0KQlQNCjAgMCAwIHJnDQovRjEgMDAyNyBUZg0KNTcuMzc1MCA3MjIuMjgwMCBUZA0KKCBTaW1wbGUgUERGIEZpbGUgMiApIFRqDQpFVA0KQlQNCi9GMSAwMDEwIFRmDQo2OS4yNTAwIDY4OC42MDgwIFRkDQooIC4uLmNvbnRpbnVlZCBmcm9tIHBhZ2UgMS4gWWV0IG1vcmUgdGV4dC4gQW5kIG1vcmUgdGV4dC4gQW5kIG1vcmUgdGV4dC4gKSBUag0KRVQNCkJUDQovRjEgMDAxMCBUZg0KNjkuMjUwMCA2NzYuNjU2MCBUZA0KKCBBbmQgbW9yZSB0ZXh0LiBBbmQgbW9yZSB0ZXh0LiBBbmQgbW9yZSB0ZXh0LiBBbmQgbW9yZSB0ZXh0LiBBbmQgbW9yZSApIFRqDQpFVA0KQlQNCi9GMSAwMDEwIFRmDQo2OS4yNTAwIDY2NC43MDQwIFRkDQooIHRleHQuIE9oLCBob3cgYm9yaW5nIHR5cGluZyB0aGlzIHN0dWZmLiBCdXQgbm90IGFzIGJvcmluZyBhcyB3YXRjaGluZyApIFRqDQpFVA0KQlQNCi9GMSAwMDEwIFRmDQo2OS4yNTAwIDY1Mi43NTIwIFRkDQooIHBhaW50IGRyeS4gQW5kIG1vcmUgdGV4dC4gQW5kIG1vcmUgdGV4dC4gQW5kIG1vcmUgdGV4dC4gQW5kIG1vcmUgdGV4dC4gKSBUag0KRVQNCkJUDQovRjEgMDAxMCBUZg0KNjkuMjUwMCA2NDAuODAwMCBUZA0KKCBCb3JpbmcuICBNb3JlLCBhIGxpdHRsZSBtb3JlIHRleHQuIFRoZSBlbmQsIGFuZCBqdXN0IGFzIHdlbGwuICkgVGoNCkVUDQplbmRzdHJlYW0NCmVuZG9iag0KDQo4IDAgb2JqDQpbL1BERiAvVGV4dF0NCmVuZG9iag0KDQo5IDAgb2JqDQo8PA0KL1R5cGUgL0ZvbnQNCi9TdWJ0eXBlIC9UeXBlMQ0KL05hbWUgL0YxDQovQmFzZUZvbnQgL0hlbHZldGljYQ0KL0VuY29kaW5nIC9XaW5BbnNpRW5jb2RpbmcNCj4+DQplbmRvYmoNCg0KMTAgMCBvYmoNCjw8DQovQ3JlYXRvciAoUmF2ZSBcKGh0dHA6Ly93d3cubmV2cm9uYS5jb20vcmF2ZVwpKQ0KL1Byb2R1Y2VyIChOZXZyb25hIERlc2lnbnMpDQovQ3JlYXRpb25EYXRlIChEOjIwMDYwMzAxMDcyODI2KQ0KPj4NCmVuZG9iag0KDQp4cmVmDQowIDExDQowMDAwMDAwMDAwIDY1NTM1IGYNCjAwMDAwMDAwMTkgMDAwMDAgbg0KMDAwMDAwMDA5MyAwMDAwMCBuDQowMDAwMDAwMTQ3IDAwMDAwIG4NCjAwMDAwMDAyMjIgMDAwMDAgbg0KMDAwMDAwMDM5MCAwMDAwMCBuDQowMDAwMDAxNTIyIDAwMDAwIG4NCjAwMDAwMDE2OTAgMDAwMDAgbg0KMDAwMDAwMjQyMyAwMDAwMCBuDQowMDAwMDAyNDU2IDAwMDAwIG4NCjAwMDAwMDI1NzQgMDAwMDAgbg0KDQp0cmFpbGVyDQo8PA0KL1NpemUgMTENCi9Sb290IDEgMCBSDQovSW5mbyAxMCAwIFINCj4+DQoNCnN0YXJ0eHJlZg0KMjcxNA0KJSVFT0YNCg";
    char* pdf2 = "hahahahahahahahahahhhhhhhhhhhhhhhhhhhhhh";
    char* pdf3 = "hahahahahahahahahahhhhhhhhhhhhhhhhhhhhhh";
    insertBlockchainData(studentCode1, firstNameStudent1, lastNameStudent1, courseNumber1, trimester1, courseName1, grade1, blockId1);
    insertBlockchainData(studentCode1, firstNameStudent1, lastNameStudent1, courseNumber1, trimester2, courseName2, grade2, blockId2);

    insertBlockchainData(studentCode2, firstNameStudent2, lastNameStudent2, courseNumber1, trimester1, courseName1, grade2, blockId1);
    insertBlockchainData(studentCode2, firstNameStudent2, lastNameStudent2, courseNumber2, trimester2, courseName2, grade1, blockId2);
    insertPdfInCourseTable(pdf, courseNumber1, trimester1);

    string pdfSelected = selectPdfFromCourse(courseNumber1, trimester1);
    cout << pdfSelected << endl;

    list<Json::Value> courseList = selectCoursesFromStudentCode(studentCode1, trimester1, courseNumber1);
    cout << courseList.back()["nom"] << endl;
    cout << courseList.back()["sigle"] << endl;
    cout << courseList.back()["trimestre"] << endl;
    cout << courseList.back()["note"] << endl;

    cout << courseList.front()["nom"] << endl;
    cout << courseList.front()["sigle"] << endl;
    cout << courseList.front()["trimestre"] << endl;
    cout << courseList.front()["note"] << endl;

    list<Json::Value> studentList = selectStudentsFromCourseNumber(courseNumber1, trimester1);

    cout << studentList.front()["nom"] << endl;
    cout << studentList.front()["prenom"] << endl;
    cout << studentList.front()["matricule"]    << endl;
    cout << studentList.front()["note"] << endl;

    insertBlockchainData(studentCode2, firstNameStudent2, lastNameStudent2, courseNumber2, trimester2, courseName2, updateGrade, updateBlockId);

    list<Json::Value> courseList2 = selectCoursesFromStudentCode(studentCode2);
    cout << courseList2.back()["nom"] << endl;
    cout << courseList2.back()["sigle"] << endl;
    cout << courseList2.back()["trimestre"] << endl;
    cout << courseList2.back()["note"] << endl;

    int blockId = selectBlockIdFromCourses(courseNumber1, trimester1);
    cout << blockId << endl;
}

void testMinerRequest(MinerCommunication& minerCommunicator){
    Value val;
    val["Hey"] = "hello";
    Request req;
    Value res;
    req.data = val;
    req.type = 0;
    minerCommunicator.sendMinerRequest(req, &res);
    sleep(2);
}

void testSQLRequest() {
    Value responseStudentList;
    Value responseCourseList;
    Value dataToFind;
    dataToFind["sigle"] = "INF3995";
    dataToFind["trimestre"] = 20193;
    dataToFind["matricule"] = 1234567;

    char* courseNumberToFind = strdup(dataToFind["sigle"].asString().c_str());
    int studentIndex = 0;
    int courseIndex = 0;
    
    studentList = selectStudentsFromCourseNumber(courseNumberToFind,
                                                stoi(dataToFind["trimestre"].asString()));

    cout << "looking in the database for a student list" << endl;
    int index = 0;
    for(std::list<Json::Value>::iterator it = studentList.begin(); it != studentList.end(); ++it){
        responseStudentList["resultats"][index++] = *it;
    }

    classList = selectCoursesFromStudentCode(stoi(dataToFind["matricule"].asString()),
                                           stoi(dataToFind["trimestre"].asString()),
                                           strdup(dataToFind["sigle"].asString().c_str())
                                           );

    cout << "looking in the database for a course list" << endl;

    index = 0;
    for(std::list<Json::Value>::iterator it = classList.begin(); it != classList.end(); ++it){
        responseCourseList["resultats"][index++] = *it;
    }

    cout << "after sql search:" << endl;
    studentIndex = selectBlockIdFromCourses(courseNumberToFind,
                                            stoi(dataToFind["trimestre"].asString()));

    courseIndex = selectBlockIdFromCourseAndStudent(stoi(dataToFind["matricule"].asString()),
                                                    courseNumberToFind,
                                                    stoi(dataToFind["trimestre"].asString()));

    cout << "this is the studentIndex : " << studentIndex << endl;
    cout << "this is the courseIndex : " << courseIndex << endl; 

    cout << "from studentList: " << endl;
    cout << responseStudentList << endl;
    cout << "from courseList: " << endl;
    cout << responseCourseList << endl;
}
