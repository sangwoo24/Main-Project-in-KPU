package com.example.todolist;

import androidx.appcompat.app.AppCompatActivity;
import androidx.recyclerview.widget.DividerItemDecoration;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import android.annotation.SuppressLint;
import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;

import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Calendar;
import java.util.Date;

public class MainActivity extends AppCompatActivity {
    public static Context mainContext;
    public static ArrayList<TodoList> todoLists;
    public static CustomAdapter mAdapter;
    int day,currentTodoCount, totalTodoCount;

    Button createTodo;
    EditText input;
    TextView text;

    @SuppressLint("SetTextI18n")
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        //날짜 받아오기
        Intent intent = getIntent();
        setTitle(intent.getStringExtra("date"));
        day = intent.getIntExtra("day",0);

        //todocount
        totalTodoCount = intent.getIntExtra("total",0);
        currentTodoCount = intent.getIntExtra("current",0);

        //Dot Matrix
        todoCountDot();

        //TodoCount Text
        text = findViewById(R.id.textTodoCount);
        setTodoCountText(totalTodoCount, currentTodoCount);

        //LED
        todoCountLed();

        mainContext = this;

        createTodo = findViewById(R.id.createTodo);
        input = findViewById(R.id.input);

        RecyclerView mRecyclerView = (RecyclerView) findViewById(R.id.recyclerview_main_list);
        LinearLayoutManager mLinearLayoutManager = new LinearLayoutManager(this);
        mRecyclerView.setLayoutManager(mLinearLayoutManager);

        todoLists = new ArrayList<>();
        mAdapter = new CustomAdapter(todoLists);
        mRecyclerView.setAdapter(mAdapter);


        DividerItemDecoration dividerItemDecoration = new DividerItemDecoration(mRecyclerView.getContext(),
                mLinearLayoutManager.getOrientation());
        mRecyclerView.addItemDecoration(dividerItemDecoration);

        createTodo.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                if(!input.getText().toString().equals("")){
                    addTodo(input.getText().toString());
                    input.setText("");
                    currentTodoCount += 1;
                    setTodoCountText(totalTodoCount, currentTodoCount);
                    todoCountLed();
                    todoCountDot();
                }
            }
        });
        mAdapter.notifyDataSetChanged();

        class Clock implements Runnable{
            Calendar cal = Calendar.getInstance();
            @SuppressLint("SimpleDateFormat")
            SimpleDateFormat sdf = new SimpleDateFormat("MM/dd HH:mm:ss");

            @Override
            public void run(){
                while(true){
                    Date now = new Date();
                    String time = sdf.format(now);
                    String todoCount = "Today's Todo : " + String.valueOf(totalTodoCount);
                    textLcd(time,todoCount);
                    try{
                        Thread.sleep(1000);
                    }catch (Exception e){
                        e.printStackTrace();
                    }
                }
            }
        }

        ArrayList<TodoList> tt = ((CalendarActivity) CalendarActivity.calendarContext).getDayTodoList(day);
        if(tt != null){
            for(int i = 0 ; i < tt.size(); i++){
                addTodo(tt.get(i));
            }
        }

        mAdapter.notifyDataSetChanged();
//        Clock clock = new Clock();
//        Thread clockThread = new Thread(clock);
//        clockThread.start();

    }

    static {
        System.loadLibrary("native-lib");
    }

    @SuppressLint("SetTextI18n")
    void setTodoCountText(int total, int current){
        text.setText("Total Todo : " + total + "   Current Todo : " + current);
    }
    void addTodo(TodoList todo){
        todoLists.add(todo);
        mAdapter.notifyDataSetChanged();
    }

    void addTodo(String s){
        totalTodoCount += 1;
        TodoList todo = new TodoList(false,s);
        todoLists.add(todo);
        mAdapter.notifyDataSetChanged();
    }

    void deleteTodo(int index){
        totalTodoCount -= 1;
        todoLists.remove(index);
        mAdapter.notifyDataSetChanged();
        if(currentTodoCount > 0)
            currentTodoCount -= 1;
        setTodoCountText(totalTodoCount,currentTodoCount);
        todoCountDot();
        todoCountLed();
    }

    void setItemIndexToLast(TodoList todo){
        todoLists.remove(todo);
        todoLists.add(todo);
        todo.isFinished = true;
        mAdapter.notifyDataSetChanged();
        currentTodoCount -= 1;
        setTodoCountText(totalTodoCount,currentTodoCount);
        todoCountDot();
        todoCountLed();
    }

    void setItemIndexToFirst(TodoList todo){
        todoLists.remove(todo);
        todoLists.add(0,todo);
        todo.isFinished = false;
        mAdapter.notifyDataSetChanged();
        currentTodoCount += 1;
        setTodoCountText(totalTodoCount,currentTodoCount);
        todoCountDot();
        todoCountLed();
    }

    public void onPause(){
        super.onPause();
        removeDotAndLed();
        ((CalendarActivity) CalendarActivity.calendarContext).getTodoLists(todoLists,day);
    }

    public void todoCountLed(){
        //Percent : LED
        double percent = ((totalTodoCount - currentTodoCount) / (double)totalTodoCount) * 100;
        System.out.println(percent);
        if(percent == 0 || Double.isNaN(percent)) {
            led(0);
        }else if(percent > 0 && percent < 50){
            led(1);
        }else if(percent >= 50 && percent < 75){
            led(2);
        }else if(percent >=75 && percent < 100){
            led(3);
        }else if(percent == 100){
            led(4);
        }
    }
    public void todoCountDot(){
        if(currentTodoCount <= 9)
            dot(currentTodoCount);
    }

    public void removeDotAndLed(){
        dot(0);
        led(0);
    }

    public native int dot(int x);
    public native int led(int x);
    public native int textLcd(String s1, String s2);


    static{
        System.loadLibrary("native-lib");
    }
}


