#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FILE_NAME "studentData.txt"
#define MAX_NO_STUDENTS 25
#define MAX_NO_COURSES 4

struct Course{
  char course_name[10];
  char course_grade;
  int course_credit;
};

struct Student{
  int std_rollno;
  char std_name[50];
  char dept[10];
  float gpa;
  int no_of_courses;
  struct Course course_list[MAX_NO_COURSES];
};

struct Student student_list[MAX_NO_STUDENTS];
int student_count = 0;

int calculateScore(char grade){
  switch (grade){
  case 'S':
    return 10;
  case 'A':
    return 9;
  case 'B':
    return 8;
  case 'C':
    return 7;
  case 'D':
    return 6;
  case 'E':
    return 5;
  case 'F':
    return 0;
  default:
    return 0;
  };
}

void calculateGpa(struct Student *student){
  int points = 0;
  int credits = 0;
  for (int i = 0; i < student->no_of_courses; i++){
    points += calculateScore(student->course_list[i].course_grade) * student->course_list[i].course_credit;
    credits += student->course_list[i].course_credit;
  }
  if (credits > 0)
    student->gpa = (float)(points / credits);
  else
    student->gpa = 0.0;
}

void readStudentData(char *filename){
  FILE *fp = fopen(filename, "r");
  if (fp == NULL){
    printf("Error opening file %s.\n", filename);
    return;
  }
  student_count = 0;
  while (fscanf(fp, "%d %s %s %d", &student_list[student_count].std_rollno, student_list[student_count].std_name,
                student_list[student_count].dept, &student_list[student_count].no_of_courses) == 4){
    for (int i = 0; i < student_list[student_count].no_of_courses; i++){
      fscanf(fp, "%s %d %c", student_list[student_count].course_list[i].course_name,
             &student_list[student_count].course_list[i].course_credit, &student_list[student_count].course_list[i].course_grade);
    }
    student_count++;
    calculateGpa(&student_list[student_count]);
    if (student_count >= MAX_NO_STUDENTS){
      printf("Cannot add new student as maximum number of students reached.\n");
      break;
    }
  }
  fclose(fp);
}

void writeStudentData(char *filename){
  FILE *fp = fopen(filename, "w");
  if (fp == NULL){
    printf("Error opening file %s.\n", filename);
    return;
  }
  for (int i = 0; i < student_count; i++){
    fprintf(fp,"--------------------\n");
    fprintf(fp, "Name: %s  | Roll number : %d\n", student_list[i].std_name, student_list[i].std_rollno);
    fprintf(fp,"--------------------\n");
    for (int j = 0; j < student_list[i].no_of_courses; j++)
      fprintf(fp, "%-10s |   %c   \n", student_list[i].course_list[j].course_name, student_list[i].course_list[j].course_grade);
    fprintf(fp,"-------------------\n");
    fprintf(fp, "GPA        | %.2f \n", student_list[i].gpa);
    fprintf(fp,"--------------------\n");
  }
  fclose(fp);
  printf("New student data stored in %s.\n", filename);
}

void addStudentData(char *filename, struct Student *student){
  FILE *fp = fopen(filename, "a");
  if (fp == NULL){
    printf("Error opening file %s.\n", filename);
    return;
  }
  fprintf(fp, "--------------------\n");
  fprintf(fp, "Name: %s  | Roll number : %d\n", student->std_name, student->std_rollno);
  fprintf(fp, "--------------------\n");
  for (int i = 0; i < student->no_of_courses; i++)
    fprintf(fp, "%-10s |   %c   \n", student->course_list[i].course_name, student->course_list[i].course_grade);
  fprintf(fp, "-------------------\n");
  fprintf(fp, "GPA        | %.2f  \n", student->gpa);
  fprintf(fp, "-------------------\n");
  fclose(fp);
  printf("New Student data added to file %s.\n", filename);
}

void deleteStudentData(char *filename, int rollno){
  FILE *fp = fopen(filename, "r");
  if (fp == NULL){
    printf("Error opening file %s.\n", filename);
    return;
  }
  FILE *t_fp = fopen("tempStudentData.txt", "w");
  if (t_fp == NULL){
    fclose(fp);
    printf("Error creating file.\n");
    return;
  }
  int flag = 0;
  char line[256];
  while (fgets(line, sizeof(line), fp)){
    int temp_roll;
    sscanf(line, "%d", &temp_roll);
    if (temp_roll == rollno){
      flag = 1;
      continue; 
    }
    fputs(line, t_fp);
  }
  fclose(fp);
  fclose(t_fp);

  if (flag){
    remove(filename);
    rename("tempStudentData.txt", filename);
    printf("Student with roll number %d deleted from file.\n", rollno);
  }
  else{
    remove("tempStudentData.txt");
    printf("Student with roll number %d not found in file.\n", rollno);
  }
}

void addGpaColumn(){
  for (int i = 0; i < student_count; i++)
    calculateGpa(&student_list[i]);
  printf("GPA column created for all students.\n");
}

void addNewStudent(){
  if (student_count >= MAX_NO_STUDENTS){
    printf("Student limit exceeded.\n");
    return;
  }
  struct Student *student = &student_list[student_count++];
  printf("Enter student roll number: ");
  scanf("%d", &student->std_rollno);
  printf("Enter student name: ");
  scanf("%s", student->std_name);
  printf("Enter student's department: ");
  scanf("%s", student->dept);
  printf("Enter number of courses (3 to 4): ");
  scanf("%d", &student->no_of_courses);
  for (int i = 0; i < student->no_of_courses; i++){
    printf("Enter %s's course %d name: ",student->std_name, i + 1);
    scanf("%s", student->course_list[i].course_name);
    printf("Enter %s's course %d credit: ",student->std_name, i + 1);
    scanf("%d", &student->course_list[i].course_credit);
    printf("Enter %s's course %d grade: ",student->std_name, i + 1);
    scanf(" %c", &student->course_list[i].course_grade);
  }
  calculateGpa(student);
}

void insertNewCourse(int rollno, char *course_name, int credit, char grade){
  for (int i = 0; i < student_count; i++){
    if (student_list[i].std_rollno == rollno){
      if (student_list[i].no_of_courses >= MAX_NO_COURSES){
        printf("%s has %d courses, Cant add more courses.\n",student_list[i].std_name,MAX_NO_COURSES);
        return;
      }
      struct Course *course = &student_list[i].course_list[student_list[i].no_of_courses++];
      strcpy(course->course_name, course_name);
      course->course_credit = credit;
      course->course_grade = grade;
      calculateGpa(&student_list[i]);
      printf("Course %s added for student %d.\n", course_name, rollno);
      return;
    }
  }
  printf("Roll no %d do not exist.\n",rollno);
}

void updateCourseName(int rollno, char *old_name, char *new_name){
  for (int i = 0; i < student_count; i++){
    if (student_list[i].std_rollno == rollno){
      for (int j = 0; j < student_list[i].no_of_courses; j++){
        if (strcmp(student_list[i].course_list[j].course_name, old_name) == 0){
          strcpy(student_list[i].course_list[j].course_name, new_name);
          printf("Course name updated from %s to %s for student %d.\n", old_name, new_name, rollno);
          return;
        }
      }
    }
  }
  printf("Course with %s not found for the student %d.\n",old_name,rollno);
}

void deleteCourse(int rollno, char *course_name){
  for (int i = 0; i < student_count; i++){
    if (student_list[i].std_rollno == rollno){
      for (int j = 0; j < student_list[i].no_of_courses; j++){
        if (strcmp(student_list[i].course_list[j].course_name, course_name) == 0){
          for (int k = j; k < student_list[i].no_of_courses - 1; k++)
            student_list[i].course_list[k] = student_list[i].course_list[k + 1];
          student_list[i].no_of_courses--;
          calculateGpa(&student_list[i]);
          printf("Course %s deleted for student %d.\n", course_name, rollno);
          return;
        }
      }
    }
  }
  printf("Enter student roll no or course name correctly\n");
}

void upgradeGrade(char grade, int new_points){
  for (int i = 0; i < student_count; i++){
    for (int j = 0; j < student_list[i].no_of_courses; j++){
      if (student_list[i].course_list[j].course_grade == grade)
        student_list[i].course_list[j].course_grade = new_points;
    }
    calculateGpa(&student_list[i]);
  }
  printf("Grades upgraded successfully.\n");
}

void upgradeGpa(int rollno){
  for (int i = 0; i < student_count; i++){
    if (student_list[i].std_rollno == rollno){
      calculateGpa(&student_list[i]);
      printf("GPA upgraded for student %d.\n", rollno);
      return;
    }
  }
  printf("Student not found.\n");
}

void generateGradeReport(int rollno){
  for (int i = 0; i < student_count; i++){
    if (student_list[i].std_rollno == rollno){
      printf("Grade report |   Student %d:\n", rollno);
      printf("Course       |   Grade\n");
      printf("------------------\n");
      for (int j = 0; j < student_list[i].no_of_courses; j++)
        printf("%-10s  |   %c    \n", student_list[i].course_list[j].course_name, student_list[i].course_list[j].course_grade);
      printf("-------------------\n");
      printf("GPA       |   %.2f \n", student_list[i].gpa);
      return;
    }
  }
  printf("Student not found.\n");
}

int main(){
  int choice;
  int rollno;
  char course_name[10];
  int credit;
  char grade;
  char *filename = FILE_NAME; 
  readStudentData(filename);
  do{
    printf("\n\nStudent Database Menu : \n");
    printf("------------------------\n");
    printf("1. To Create new student\n");
    printf("2. To add a GPA column\n");
    printf("3. To delete a course\n");
    printf("4. To insert a course\n");
    printf("5. To rename the course name\n");
    printf("6. To calculate GPA for students\n");
    printf("7. To upgrade grade\n");
    printf("8. To upgrade GPA for a student\n");
    printf("9. To generate grade report\n");
    printf("10. Exit\n");
    printf("Enter your choice: ");
    scanf("%d", &choice);
    switch (choice){
      case 1:
        addNewStudent();
        addStudentData(filename, &student_list[student_count - 1]); 
        break;
      case 2:
        addGpaColumn();
        break;
      case 3:
        printf("Enter the roll number: ");
        scanf("%d", &rollno);
        printf("Enter the course name: ");
        scanf("%s", course_name);
        deleteCourse(rollno, course_name);
        writeStudentData(filename); 
        break;
      case 4:
        printf("Enter the roll number: ");
        scanf("%d", &rollno);
        printf("Enter the course name: ");
        scanf("%s", course_name);
        printf("Enter credit score: ");
        scanf("%d", &credit);
        printf("Enter grade score: ");
        scanf(" %c", &grade);
        insertNewCourse(rollno, course_name, credit, grade);
        writeStudentData(filename);
        break;
      case 5:
        char prev_name[10];
        char new_name[10];
        printf("Enter the roll number: ");
        scanf("%d", &rollno);
        printf("Enter the previous course name: ");
        scanf("%s", prev_name);
        printf("Enter the new course name: ");
        scanf("%s", new_name);
        updateCourseName(rollno, prev_name, new_name);
        writeStudentData(filename); 
        break;
      case 6:
        addGpaColumn();
        break;
      case 7:
        int points;
        printf("Enter the upgraded grade: ");
        scanf(" %c", &grade);
        printf("Enter points: ");
        scanf("%d", &points);
        upgradeGrade(grade, points);
        writeStudentData(filename); 
        break;
      case 8:
        printf("Enter the roll number: ");
        scanf("%d", &rollno);
        upgradeGpa(rollno);
        writeStudentData(filename); 
        break;
      case 9:
        printf("Enter the roll number: ");
        scanf("%d", &rollno);
        generateGradeReport(rollno);
        break;
      case 10:
        printf("Thank you for using the student database...\n");
        break;
      default:
        printf("ERROR : Plese select a choice between 1 to 10.\n");
    };
  } while (choice != 10);
}
